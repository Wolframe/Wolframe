#include "database/vmTransactionInput.hpp"
#include "vm/program.hpp"
#include "vm/selectorPathSet.hpp"
#include "utils/printFormats.hpp"
#include "vm/inputStructure.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

using namespace _Wolframe;
using namespace _Wolframe::db;

void VmTransactionInput::print( std::ostream& out) const
{
	m_program.print( out);
}

std::string VmTransactionInput::tostring() const
{
	std::ostringstream out;
	print( out);
	return out.str();
}

namespace {
struct ProgramRewriter
	:public vm::InstructionSet
{
	struct StackElem
	{
		Address ip;
		std::vector<vm::InputNodeIndex> selected;

		StackElem():ip(0){}
		StackElem( const StackElem& o)
			:ip(o.ip),selected(o.selected){}
		StackElem( Address ip_, const std::vector<vm::InputNodeIndex>& selected_)
			:ip(ip_),selected(selected_){}
	};

	static void rewriteInputPathReferences( vm::ProgramImage& prg, const vm::SelectorPathSet& pathset, const vm::InputStructure& input);
};


void ProgramRewriter::rewriteInputPathReferences( vm::ProgramImage& prg, const vm::SelectorPathSet& pathset, const vm::InputStructure& input)
{
	if (prg.code.size() == 0) return;

	// Initialize first element to visit:
	std::map<ArgumentIndex,bool> visited;
	std::vector<StackElem> stack;
	std::vector<vm::InputNodeIndex> root;
	root.push_back( input.rootindex());
	stack.push_back( StackElem( 0, root));

	// Visit all open elements:
	while (!stack.empty())
	{
		Instruction instr = *prg.code.at( stack.back().ip);
		OpCode oc = opCode( instr);

		if (oc == Op_EXIT)
		{
			if (condCode( instr) == Co_ALWAYS)
			{
				stack.pop_back();
			}
			else
			{
				if (++stack.back().ip >= prg.code.size())
				{
					stack.pop_back();
				}
			}
		}
		else if (oc == Op_RETURN)
		{
			if (condCode( instr) == Co_ALWAYS)
			{
				stack.pop_back();
			}
			else
			{
				if (++stack.back().ip >= prg.code.size())
				{
					stack.pop_back();
				}
			}
		}
		else if (oc == Op_GOTO)
		{
			if (condCode( instr) == Co_ALWAYS)
			{
				stack.back().ip = argumentIndex( instr);
			}
			else
			{
				ArgumentIndex ai = argumentIndex( instr);
				if (visited.find( ai) == visited.end())
				{
					visited[ ai] = true;
					std::vector<vm::InputNodeIndex> selected = stack.back().selected;
					if (++stack.back().ip >= prg.code.size())
					{
						stack.pop_back();
					}
					stack.push_back( StackElem( ai, selected));
				}
				else
				{
					if (++stack.back().ip >= prg.code.size())
					{
						stack.pop_back();
					}
				}
			}
		}
		else if (argumentType( oc) == At_Path)
		{
			if (oc == Op_OPEN_ITER_PATH)
			{
				// [1] Rewrite the instruction to reference the tuple set,
				//	that will be created in the following scope:
				oc = Op_OPEN_ITER_TUPLESET;
				prg.code[ stack.back().ip] = instruction( condCode(instr), oc, prg.tuplesets.size());

				// [2] Get the path referenced and create its set of selected nodes ('selected') in the input :
				const vm::SelectorPath& path = pathset.getPath( argumentIndex( instr));
				std::vector<vm::InputNodeIndex> selected;
				{
					std::vector<vm::InputNodeIndex>::const_iterator ni = stack.back().selected.begin(), ne = stack.back().selected.end();
					for (; ni != ne; ++ni)
					{
						path.selectNodes( input, *ni, selected);
					}
				}

				// [3] Rewite all instructions referencing path expressions
				//	in the iterator scope (till NEXT):
				Address ip = stack.back().ip;			//< instruction pointer inside the loop iterated
				std::vector<vm::SelectorPath> pathtuple;	//< tuple of sub path expressions referenced in the loop
				for (; ip < prg.code.size(); ++ip)
				{
					instr = *prg.code.at( ip);
					oc = opCode( instr);
					if (oc == Op_NEXT) break;	//... end of the FOREACH loop reached

					if (argumentType( oc) == At_Path)
					{
						// [3.1] Rewrite the path reference 
						//	instruction in the FOREACH loop:
						ArgumentIndex ai = argumentIndex( instr);
						pathtuple.push_back( pathset.getPath( ai));
						if (oc == Op_SUB_ARG_PATH)
						{
							oc = Op_SUB_ARG_ITR_IDX;
						}
						else if (oc == Op_OUTPUT_PATH)
						{
							oc = Op_OUTPUT_ITR_IDX;
						}
						else if (oc == Op_DBSTM_BIND_PATH)
						{
							oc = Op_DBSTM_BIND_ITR_IDX;
						}
						else if (oc == Op_OPEN_ITER_PATH)
						{
							throw std::runtime_error( "internal: cannot rewrite program with nested path expression loops");
						}
						else
						{
							throw std::runtime_error( "internal: unknown instruction with path argument");
						}
						prg.code[ ip] = instruction( condCode(instr), oc, pathtuple.size());
					}
				}
				// [4] Create the value tuple set for the pathes referenced:
				if (pathtuple.size() == 0)
				{
					// [4.A] ... no element (path) of the FOREACH referenced, but we have to create the set for the FOREACH loop to happen

					// [4.A.1] Create column name vector:
					std::vector<std::string> colnames;
					colnames.push_back( "");

					// [4.A.2] Create tuple set {(NULL),(NULL),....}:
					std::vector<types::Variant> vecNULL;
					vecNULL.push_back( types::Variant());
					vm::ValueTupleSetR tupleset( new vm::ValueTupleSet( colnames));
					std::vector<vm::InputNodeIndex>::const_iterator ni = selected.begin(), ne = selected.end();
					for (; ni != ne; ++ni)
					{
						tupleset->push( vecNULL);
					}
					prg.tuplesets.push_back( tupleset);
				}
				else
				{
					// [4.B] ... tuple of  elements (path) of the FOREACH referenced

					// [4.B.1] Create column name vector:
					std::vector<std::string> colnames;
					std::vector<vm::SelectorPath>::const_iterator ti = pathtuple.begin(), te = pathtuple.end();
					const char* elemname;
					for (; ti != te; ++ti)
					{
						elemname = ti->lastElementName( pathset.tagtab());
						if (!elemname && elemname[0] == '.' && !elemname[1])
						{
							elemname = path.lastElementName( pathset.tagtab());
						}
						if (elemname)
						{
							colnames.push_back( elemname);
						}
						else
						{
							colnames.push_back( "");
						}
					}
					// [4.B.2] Create tuple set {(t1,t2,...tN),....}:
					vm::ValueTupleSetR tupleset( new vm::ValueTupleSet( colnames));
					types::VariantConst null;
					std::vector<vm::InputNodeIndex>::const_iterator ni = selected.begin(), ne = selected.end();
					for (; ni != ne; ++ni)
					{
						ti = pathtuple.begin(), te = pathtuple.end();
						std::vector<types::VariantConst> valuetuple;
						for (; ti != te; ++ti)
						{
							std::vector<vm::InputNodeIndex> element_value_ar;
							ti->selectNodes( input, *ni, element_value_ar);
							if (element_value_ar.empty())
							{
								valuetuple.push_back( null);
							}
							else
							{
								if (element_value_ar.size() > 1)
								{
									std::vector<vm::InputNodeIndex>::const_iterator ei = element_value_ar.begin(), ee = element_value_ar.end();
									vm::InputNodeIndex first = *ei;
									for (++ei; ei != ee; ++ei)
									{
										if (*ei != first)
										{
											throw std::runtime_error( std::string("ambiguus value reference in loop: '") + ti->tostring( pathset.tagtab()) + "'");
										}
									}
								}
								const vm::InputNode* nd = input.node( element_value_ar.at(0));
								const types::Variant* value = input.contentvalue( nd);
								if (value)
								{
									valuetuple.push_back( types::VariantConst( *value));
								}
								else
								{
									valuetuple.push_back( null);
								}
							}
						}
						tupleset->push( valuetuple);
					}
					prg.tuplesets.push_back( tupleset);
				}
			}
			else
			{
				//... single instruction (outside FOREACH context) with path reference 

				// [1] Get the path referenced:
				const vm::SelectorPath& path = pathset.getPath( argumentIndex( instr));
				std::vector<vm::InputNodeIndex> selected;
				std::vector<vm::InputNodeIndex>::const_iterator ni = stack.back().selected.begin(), ne = stack.back().selected.end();
				for (; ni != ne; ++ni)
				{
					path.selectNodes( input, *ni, selected);
				}
				// [2] Get the value (unique) of its selected node in the input as constant:
				ArgumentIndex constArgumentIndex = prg.constants.size();
				if (selected.empty())
				{
					prg.constants.push_back( types::Variant());
				}
				else if (selected.size() > 1)
				{
					throw std::runtime_error( std::string("ambiguus single value reference: '") + path.tostring( pathset.tagtab()) + "'");
				}
				else
				{
					vm::InputNodeIndex valueidx = selected.back();
					const vm::InputNode* nd = input.node( valueidx);
					const types::Variant* value = input.contentvalue( nd);
					if (value)
					{
						prg.constants.push_back( types::VariantConst( *value));
					}
					else
					{
						prg.constants.push_back( types::Variant());
					}
				}
				// [3] Get the instruction opcode:
				if (oc == Op_SUB_ARG_PATH)
				{
					oc = Op_SUB_ARG_CONST;
				}
				else if (oc == Op_OUTPUT_PATH)
				{
					oc = Op_OUTPUT_CONST;
				}
				else if (oc == Op_DBSTM_BIND_PATH)
				{
					oc = Op_DBSTM_BIND_CONST;
				}
				else
				{
					throw std::runtime_error( "internal: unknown instruction with path argument");
				}
				// [4] Rewrite the instruction to reference the tuple set that will be created in the following scope:
				prg.code[ stack.back().ip] = instruction( condCode(instr), oc, constArgumentIndex);
			}
			if (++stack.back().ip >= prg.code.size())
			{
				stack.pop_back();
			}
		}
		else
		{
			if (++stack.back().ip >= prg.code.size())
			{
				stack.pop_back();
			}
		}
	}
}
}//anonymous namespace


VmTransactionInput::VmTransactionInput( const vm::Program& p, const vm::InputStructure& input)
	:m_program(p)
{
	ProgramRewriter::rewriteInputPathReferences( m_program, p.pathset, input);
}


