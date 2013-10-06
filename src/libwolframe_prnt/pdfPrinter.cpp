/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt_pdfPrinter.hpp
///\brief Implements the prnt::PrintFunction base on libhpdf
#include "prnt/pdfPrinter.hpp"
#include "prnt/pdfPrinterVariable.hpp"
#include "prnt/pdfPrinterMethod.hpp"
#include "prnt/pdfPrinterExpression.hpp"
#include "prnt/pdfPrinterDocument.hpp"
#include "utils/parseUtils.hpp"
#include "textwolf/xmlpathautomatonparse.hpp"
#include "textwolf/xmlpathselect.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

static bool isSpace( char ch)
{
	return ((unsigned char)ch <= (unsigned char)' ');
}

static bool isEmpty( const std::string& line)
{
	std::string::const_iterator itr = line.begin();
	const std::string::const_iterator end = line.end();
	for (; itr != end; ++itr) if (*itr < 0 || *itr > 32) return false;
	return true;
}

static std::string getSelectionExpression( std::string::const_iterator& itr, const std::string::const_iterator& end)
{
	std::string::const_iterator eb = itr;
	for (; itr != end; ++itr)
	{
		if (*itr == ':')
		{
			std::string::const_iterator ee = itr;
			++itr;
			if (itr != end && isSpace(*itr))
			{
				++itr;
				return std::string( eb, ee);
			}
		}
	}
	return std::string( eb, end);
}

typedef textwolf::XMLPathSelectAutomatonParser<textwolf::charset::UTF8,textwolf::charset::UTF8> XMLPathSelectAutomatonParser;
typedef textwolf::XMLPathSelect<textwolf::charset::UTF8> XMLPathSelect;

struct HaruPdfPrintFunction::Impl
{
public:
	Impl( const std::string& src, CreateDocumentFunc createDocument_)
		:m_createDocument(createDocument_)
	{
		m_exprstrings.push_back( '\0');
		std::string::const_iterator itr = src.begin(), end=src.end();
		try
		{
			while (utils::gotoNextToken( itr, end) == '!')
			{
				++itr;
				std::pair<std::string,std::string> assignment = utils::parseTokenAssignement( itr, end);
				m_attributes[ boost::algorithm::to_lower_copy( assignment.first)] = assignment.second;
			}
			while (itr != end)
			{
				std::string line = utils::parseNextLine( itr, end);
				if (isEmpty( line)) continue;

				std::string::const_iterator li=line.begin(), le=line.end();
				std::string xpathstr = getSelectionExpression( li, le);
				int xerr = m_parser.addExpression( (int)m_statedef.size()+1, xpathstr.c_str(), xpathstr.size());
				if (xerr != 0)
				{
					std::runtime_error( std::string( "in xml selection path at position ") + boost::lexical_cast<std::string>(xerr));
				}
				StateDef stateDef;
				stateDef.parse( li, le, m_exprstrings);
				m_statedef.push_back( stateDef);
			}
		}
		catch (const std::runtime_error& e)
		{
			utils::LineInfo pos = utils::getLineInfo( src.begin(), itr);
			throw std::runtime_error( std::string( "error on line ") + boost::lexical_cast<std::string>(pos.line) + " of PDF printer layout description source (" + e.what() + ")");
		}
	}

	const StateDef* statedef( std::size_t idx) const	{return &m_statedef[idx];}
	const std::string& exprstrings() const			{return m_exprstrings;}
	const XMLPathSelectAutomatonParser& parser() const	{return m_parser;}
	Document* createDocument() const			{return m_createDocument();}

	const std::string& attribute( const std::string n_) const
	{
		static const std::string empty;
		std::map<std::string,std::string>::const_iterator ai = m_attributes.find( n_);
		return ai == m_attributes.end() ? empty:ai->second;
	}

	std::string tostring() const
	{
		std::ostringstream out;
		std::map<std::string,std::string>::const_iterator ai = m_attributes.begin(), ae = m_attributes.end();
		for (; ai != ae; ++ai)
		{
			out << "ATTRIBUTE " << ai->first << " = '" << ai->second << "'" << std::endl;
		}
		std::vector<StateDef>::const_iterator is = m_statedef.begin(), es = m_statedef.end();
		for (; is != es; ++is)
		{
			out << "STATE " << (int)(is-m_statedef.begin()) << ": " << is->tostring( m_exprstrings) << std::endl;
		}
		out << "AUTOMATON:" << std::endl;
		out << m_parser.tostring() << std::endl;

		return out.str();
	}

private:
	XMLPathSelectAutomatonParser m_parser;
	std::vector<StateDef> m_statedef;
	std::string m_exprstrings;
	CreateDocumentFunc m_createDocument;
	std::map<std::string,std::string> m_attributes;
};

HaruPdfPrintFunction::HaruPdfPrintFunction( const std::string& description, CreateDocumentFunc createDocument)
	:m_impl( new Impl( description, createDocument)){}

HaruPdfPrintFunction::~HaruPdfPrintFunction()
{
	delete m_impl;
}


class PrintInput :public langbind::TypedOutputFilter
{
public:
	PrintInput( const HaruPdfPrintFunction::Impl* func)
		:types::TypeSignature("langbind::TypedOutputFilter (pdfPrinter)", __LINE__)
		,m_document( func->createDocument())
		,m_func(func)
		,m_selectState(&func->parser())
		,m_lasttype( langbind::FilterBase::OpenTag){}

	PrintInput( const PrintInput& o)
		:types::TypeSignature("langbind::TypedOutputFilter (pdfPrinter)", __LINE__)
		,langbind::TypedOutputFilter(o)
		,m_document(o.m_document)
		,m_variableScope(o.m_variableScope)
		,m_func(o.m_func)
		,m_selectState(o.m_selectState)
		,m_lasttype(o.m_lasttype){}

	virtual ~PrintInput(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new PrintInput(*this);}

	virtual bool print( langbind::FilterBase::ElementType type, const types::VariantConst& element)
	{
		std::string elemstr = element.tostring();
		textwolf::XMLScannerBase::ElementType xtype = textwolf::XMLScannerBase::None;
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
				xtype = textwolf::XMLScannerBase::OpenTag;
				m_variableScope.push( elemstr);
				break;

			case langbind::FilterBase::CloseTag:
			{
				xtype = textwolf::XMLScannerBase::CloseTag;
				std::vector<std::size_t>::const_iterator mi = m_variableScope.begin_marker();
				std::vector<std::size_t>::const_iterator me = m_variableScope.end_marker();
				for (; mi != me; ++mi)
				{
					m_document->execute_leave( (Method::Id)*mi, m_variableScope);
				}
				m_variableScope.pop();
				break;
			}

			case langbind::FilterBase::Attribute:
				xtype = textwolf::XMLScannerBase::TagAttribName;
				break;

			case langbind::FilterBase::Value:
				if (m_lasttype == langbind::FilterBase::Attribute)
				{
					xtype = textwolf::XMLScannerBase::TagAttribValue;
				}
				else
				{
					xtype = textwolf::XMLScannerBase::Content;
				}
				break;
		}
		m_lasttype = type;

		// Execute methods triggered:
		XMLPathSelect::iterator itr = m_selectState.find( xtype, elemstr.c_str(), elemstr.size());
		XMLPathSelect::iterator end = m_selectState.end();
		for (; itr!=end; itr++)
		{
			if (*itr)
			{
				const StateDef* st = m_func->statedef( *itr -1);
				st->m_expr.evaluate( m_variableScope, m_func->exprstrings());
				std::vector<StateDef::MethodCall>::const_iterator ci = st->m_call.begin(), ce = st->m_call.end();
				for (; ci != ce; ++ci)
				{
					m_variableScope.push();
					ci->m_param.evaluate( m_variableScope, m_func->exprstrings());
					m_document->execute_enter( ci->m_method, m_variableScope);
					m_variableScope.pop();
					m_variableScope.push_marker( (std::size_t)ci->m_method);
				}
			}
		}
		return true;
	}

	const Document& document() const
	{
		return *m_document;
	}

private:
	boost::shared_ptr<Document> m_document;
	VariableScope m_variableScope;
	const HaruPdfPrintFunction::Impl* m_func;
	XMLPathSelect m_selectState;
	langbind::FilterBase::ElementType m_lasttype;
};

PrintFunction::InputR HaruPdfPrintFunction::getInput() const
{
	PrintFunction::InputR rt( new PrintInput( m_impl));
	return rt;
}

std::string HaruPdfPrintFunction::execute( const Input* input_) const
{
	const PrintInput* input = dynamic_cast<const PrintInput*>( input_);
	if (!input) throw std::runtime_error( "calling print pdf with incompatible input");
	return input->document().tostring();
}

std::string HaruPdfPrintFunction::tostring() const
{
	return m_impl->tostring();
}

const std::string& HaruPdfPrintFunction::name() const
{
	return m_impl->attribute("name");
}

PrintFunction* _Wolframe::prnt::createHaruPdfPrintFunction( const std::string& description, CreateDocumentFunc createDocument)
{
	return new HaruPdfPrintFunction( description, createDocument);
}


