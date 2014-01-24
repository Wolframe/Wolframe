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
///\file prnt_pdfPrinterDocument_libhpdf.cpp
#include "prnt/pdfPrinterDocument_libhpdf.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "logger-v1.hpp"
#include "hpdf.h"

using namespace _Wolframe;
using namespace _Wolframe::prnt;

namespace
{
class HpdfDocument
{
public:
	HpdfDocument()
		:m_hnd(0)
		,m_pageidx(0)
		,m_method(Method::PrintText)
		,m_methodcall(false)
		,m_cnt(0)
	{
		m_hnd = HPDF_New( error_handler, (void*)this);
		if (!m_hnd)
		{
			throw std::runtime_error( "cannot create PDF document");
		}
	}

	void execute( Method::Id m, VariableScope& vscope)
	{
		m_method = m;
		m_methodcall = true;
		switch (m_method)
		{
			case Method::DrawLine:
				page().initMode( Page::ModeGraphics);
				page().initLineStyle( *this, vscope);
				page().initPosition( *this, vscope);
				page().initSize( *this, vscope);
				HPDF_Page_LineTo( pagehnd(), m_vcache.m_positionX + m_vcache.m_sizeX, m_vcache.m_positionY + m_vcache.m_sizeY);
				break;

			case Method::DrawRectangle:
				page().initMode( Page::ModeGraphics);
				page().initLineStyle( *this, vscope);
				page().initPosition( *this, vscope);
				page().initSize( *this, vscope);
				HPDF_Page_Rectangle( pagehnd(), m_vcache.m_positionX, m_vcache.m_positionY, m_vcache.m_sizeX, m_vcache.m_sizeY);
				break;

			case Method::PrintText:
				page().initMode( Page::ModeText);
				page().initPosition( *this, vscope);
				page().showText( *this, vscope);
				break;

			case Method::AddPage:
				m_vcache.reset();
				m_pagear.push_back( Page( *this));
				m_pageidx = m_pagear.size();
				break;
		}
		m_methodcall = false;
	}

	void execute_enter( Method::Id method, VariableScope& vscope)
	{
		m_stk.push_back( ++m_cnt);
		LOG_TRACE << "enter " << methodName(method) << ": " << (int)m_cnt << "$ " << vardump(vscope);
		execute( method, vscope);
	}

	void execute_leave( Method::Id method, VariableScope& vscope)
	{
		if (m_stk.empty()) throw std::runtime_error( "internal: call of enter/leave not balanced");
		std::size_t idx = m_stk.back();
		m_stk.pop_back();
		LOG_TRACE << "leave " << methodName(method) << ": " << (int)idx << "$ " << vardump(vscope);
	}

	std::string tostring() const
	{
		std::string rt;
		enum {bufallocsize=1<<14};
		HPDF_BYTE buf[ bufallocsize];
		HPDF_UINT32 bufsize = 0;
		HPDF_SaveToStream( m_hnd);
		HPDF_STATUS status = HPDF_OK;
		while (status == HPDF_OK)
		{
			bufsize = bufallocsize;
			status = HPDF_ReadFromStream( m_hnd, buf, &bufsize);
			rt.append( (const char*)buf, bufsize * sizeof( HPDF_BYTE));
		}
		HPDF_ResetStream( m_hnd);
		if (status != HPDF_STREAM_EOF) throw_error( "could not serialize printed PDF document into string");
		return rt;
	}

private:
	static void error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void* void_this_)
	{
		HpdfDocument* this_ = (HpdfDocument*)void_this_;
		std::ostringstream msgbuf;
		if (this_->m_methodcall)
		{
			msgbuf << "error in method " << methodName(this_->m_method) << " calling hpdf (error code " << (int)error_no << ", detailed error code " << detail_no << ")";
		}
		else
		{
			msgbuf << "error printing PDF (calling hpdf error code " << (int)error_no << ", detailed error code " << detail_no << ")";
		}
		throw std::runtime_error( msgbuf.str());
	}

	void throw_error( const char* msg, const char* arg) const
	{
		std::ostringstream msgbuf;
		if (m_methodcall)
		{
			msgbuf << "error in method " << methodName(m_method) << ": " << msg << " (" << arg << ")";
		}
		else
		{
			msgbuf << "error printing PDF: " << msg << " (" << arg << ")";
		}
		throw std::runtime_error( msgbuf.str());
	}

	void throw_error( const char* msg) const
	{
		std::ostringstream msgbuf;
		if (m_methodcall)
		{
			msgbuf << "error in method " << methodName(m_method) << ": " << msg;
		}
		else
		{
			msgbuf << "error printing PDF: " << msg;
		}
		throw std::runtime_error( msgbuf.str());
	}

	HPDF_Doc hnd() const
	{
		return m_hnd;
	}

	enum Aligment
	{
		AlignLeft,
		AlignCenter,
		AlignRight
	};

	///\class Font
	///\brief Font definition
	///\remark Syntax: "font;encoding" no spaces
	struct Font
	{
		HPDF_Font m_hnd;
		std::string m_name;
		std::string m_encoding;

		Font()	:m_hnd(0){}

		Font( const HpdfDocument& doc, const std::string& src)
			:m_hnd(0)
		{
			std::string::const_iterator ii=src.begin(), ee=src.end();
			for (;ii != ee; ++ii)
			{
				if (*ii == ';')
				{
					for (;ii != ee; ++ii)
					{
						m_encoding.push_back( *ii);
					}
				}
				else
				{
					m_name.push_back( *ii);
				}
			}
			m_hnd = HPDF_GetFont( doc.hnd(), m_name.c_str(), m_encoding.empty()?0:m_encoding.c_str());
		}

		Font( const Font& o)
			:m_hnd(o.m_hnd)
			,m_name(o.m_name)
			,m_encoding(o.m_encoding){}
	};

	///\class LineStyle
	///\brief Line style for drawing
	///\remark Syntax: "width;ptn1,ptn2,...,ptnN;phase" no spaces
	struct LineStyle
	{
		HPDF_REAL width;
		enum {MaxPtn=32};
		HPDF_UINT16 ptn[ MaxPtn];
		HPDF_UINT nofptn;
		HPDF_UINT phase;

		LineStyle()
		{
			memset( this, 0, sizeof( *this));	//... POD init
		}

		LineStyle( const LineStyle& o)
		{
			memcpy( this, &o, sizeof( *this));	//... POD copy
		}

		LineStyle( const HpdfDocument& doc, const std::string& src)
		{
			bool hasPhase = false;
			memset( this, 0, sizeof( *this));	//... POD init

			std::string::const_iterator ii=src.begin(), ee=src.end();
			for (;ii != ee; ++ii)
			{
				if (*ii >= '0' && *ii <= '9')
				{
					width = width * 10 + *ii - '0';
				}
				else if (*ii == ' ')
				{
					doc.throw_error( "no spaces allowed in line style definition", src.c_str());
				}
				else if (*ii == ';')
				{
					break;
				}
			}
			for (;ii != ee && nofptn < MaxPtn; ++ii)
			{
				if (*ii >= '0' && *ii <= '9')
				{
					if (hasPhase)
					{
						phase = phase * 10 + *ii - '0';
					}
					else
					{
						ptn[ nofptn] = ptn[ nofptn] * 10 + *ii - '0';
					}
				}
				else if (*ii == ',')
				{
					++nofptn;
				}
				else if (*ii == ' ')
				{
					doc.throw_error( "no spaces allowed in line style definition", src.c_str());
				}
				else if (*ii == ';')
				{
					hasPhase = true;
				}
				break;
			}
			if (ii != ee) doc.throw_error( "illegal value for line style definition", src.c_str());
		}
	};

	class Page
	{
	public:
		enum Mode
		{
			ModeNone,
			ModeText,
			ModeGraphics
		};

		explicit Page( const HpdfDocument& doc)
			:m_hnd( HPDF_AddPage( doc.hnd()))
			,m_height(HPDF_Page_GetHeight( doc.pagehnd()))
			,m_width(HPDF_Page_GetWidth( doc.pagehnd()))
			,m_mode(ModeNone)
		{}

		Page( const Page& o)
			:m_hnd(o.m_hnd)
			,m_height(o.m_height)
			,m_width(o.m_width)
			,m_mode(o.m_mode){}

		HPDF_Page hnd() const
		{
			return m_hnd;
		}

		void initFont( HpdfDocument& doc, const VariableScope& vscope)
		{
			if (m_mode != ModeText) doc.throw_error( "internal: document not in textmode");
			std::size_t fvi = vscope.getValueIdx( Variable::Font);
			if (!fvi) doc.throw_error( "variable not defined", variableName( Variable::Font));
			std::size_t svi = vscope.getValueIdx( Variable::FontSize);
			if (!svi) doc.throw_error( "variable not defined", variableName( Variable::FontSize));

			if (fvi == doc.m_vcache.m_font_varidx && svi == doc.m_vcache.m_fontsize_varidx) return;

			std::map<std::size_t,Font>::const_iterator fi = doc.m_fontmap.find(fvi), fe = doc.m_fontmap.end();
			if (fi == fe)
			{
				doc.m_fontmap[ fvi] = Font( doc, vscope.getValue( fvi));
				fi = doc.m_fontmap.find( fvi);
			}
			HPDF_Page_SetFontAndSize( m_hnd, fi->second.m_hnd, boost::lexical_cast<HPDF_REAL>( vscope.getValue( svi)));
			doc.m_vcache.m_font_varidx = fvi;
			doc.m_vcache.m_fontsize_varidx = svi;
		}

		void initLineStyle( HpdfDocument& doc, const VariableScope& vscope)
		{
			if (m_mode != ModeText) doc.throw_error( "internal: document not in textmode");
			std::size_t idx = vscope.getValueIdx( Variable::LineStyle);
			if (!idx) doc.throw_error( "variable not defined", variableName( Variable::LineStyle));

			if (idx == doc.m_vcache.m_linestyle_varidx) return;

			std::map<std::size_t,LineStyle>::const_iterator fi = doc.m_linestylemap.find(idx), fe = doc.m_linestylemap.end();
			if (fi == fe)
			{
				doc.m_linestylemap[ idx] = LineStyle( doc, vscope.getValue( idx));
				fi = doc.m_linestylemap.find( idx);
			}
			HPDF_Page_SetDash( m_hnd, fi->second.ptn, fi->second.nofptn, fi->second.phase);
			// Aba, got C4244 here ('const HPDF_UINT' to 'HPDF_REAL', possible loss of data)
			// changed width in LineStyle to real, no sure if this is correct
			HPDF_Page_SetLineWidth( m_hnd, fi->second.width);
			doc.m_vcache.m_linestyle_varidx = idx;
		}

		void initPosition( HpdfDocument& doc, const VariableScope& vscope)
		{
			std::size_t xi = vscope.getValueIdx( Variable::PositionX);
			if (!xi) doc.throw_error( "variable not defined", variableName( Variable::PositionX));
			std::size_t yi = vscope.getValueIdx( Variable::PositionY);
			if (!yi) doc.throw_error( "variable not defined", variableName( Variable::PositionY));

			if (xi == doc.m_vcache.m_positionX_varidx && yi == doc.m_vcache.m_positionY_varidx) return;

			HPDF_REAL x = doc.geti<HPDF_REAL>( vscope, xi);
			HPDF_REAL y = doc.geti<HPDF_REAL>( vscope, yi);

			if (m_mode == ModeText)
			{
				HPDF_Page_MoveTextPos( m_hnd, x, y);
			}
			else
			{
				HPDF_Page_MoveTo( m_hnd, x, y);
			}
			doc.m_vcache.m_positionX_varidx = xi;
			doc.m_vcache.m_positionY_varidx = yi;
			doc.m_vcache.m_positionX = x;
			doc.m_vcache.m_positionY = y;
		}

		void initSize( HpdfDocument& doc, const VariableScope& vscope)
		{
			std::size_t xi = vscope.getValueIdx( Variable::SizeX);
			if (!xi) doc.throw_error( "variable not defined", variableName( Variable::SizeX));
			std::size_t yi = vscope.getValueIdx( Variable::SizeY);
			if (!yi) doc.throw_error( "variable not defined", variableName( Variable::SizeY));

			if (xi != doc.m_vcache.m_sizeX_varidx)
			{
				doc.m_vcache.m_sizeX = doc.geti<HPDF_REAL>( vscope, xi);
			}
			if (yi != doc.m_vcache.m_positionY_varidx)
			{
				doc.m_vcache.m_sizeY = doc.geti<HPDF_REAL>( vscope, yi);
			}
		}

		void initAlignment( HpdfDocument& doc, const VariableScope& vscope)
		{
			std::size_t idx = vscope.getValueIdx( Variable::Align);
			if (!idx)
			{
				doc.m_vcache.m_align = AlignLeft;
			}
			else
			{
				if (idx == doc.m_vcache.m_align_varidx) return;
				std::string val = doc.geti<std::string>( vscope, idx);
				if (boost::algorithm::iequals( val, "left"))
				{
					doc.m_vcache.m_align = AlignLeft;
				}
				else if (boost::algorithm::iequals( val, "right"))
				{
					doc.m_vcache.m_align = AlignRight;
				}
				else if (boost::algorithm::iequals( val, "center"))
				{
					doc.m_vcache.m_align = AlignCenter;
				}
				else
				{
					doc.throw_error( "unknown value set for variable Align", val.c_str());
				}
				doc.m_vcache.m_align_varidx = idx;
			}
		}

		void initMode( Mode mode_)
		{
			switch (mode_)
			{
				case ModeGraphics:
				case ModeNone:
					switch (m_mode)
					{
						case ModeText: HPDF_Page_EndText( m_hnd); break;
						case ModeNone:
						case ModeGraphics: break;
					}
					break;
				case ModeText:
					switch (m_mode)
					{
						case ModeText: break;
						case ModeNone:
						case ModeGraphics: HPDF_Page_BeginText( m_hnd); break;
					}
					break;
			}
			m_mode = mode_;
		}

		void showText( HpdfDocument& doc, const VariableScope& vscope)
		{
			std::string text = doc.get<std::string>( vscope, Variable::Text);
			initAlignment( doc, vscope);
			HPDF_REAL xx,yy,width;

			switch (doc.m_vcache.m_align)
			{
				case AlignLeft:
					HPDF_Page_ShowText( m_hnd, text.c_str());
					break;
				case AlignCenter:
					width = HPDF_Page_TextWidth( m_hnd, text.c_str());
					xx = doc.m_vcache.m_positionX - width / 2;
					yy = doc.m_vcache.m_positionY;
					HPDF_Page_TextOut( m_hnd, xx, yy, text.c_str());
					break;
				case AlignRight:
					width = HPDF_Page_TextWidth( m_hnd, text.c_str());
					xx = doc.m_vcache.m_positionX - width;
					yy = doc.m_vcache.m_positionY;
					HPDF_Page_TextOut( m_hnd, xx, yy, text.c_str());
					break;
			}
		}

	private:
		HPDF_Page m_hnd;
		HPDF_REAL m_height;
		HPDF_REAL m_width;
		Mode m_mode;
	};

	template <typename ValueType>
	ValueType get( const VariableScope& vscope, Variable::Id var, const ValueType& defaultValue) const
	{
		std::size_t idx = vscope.getValueIdx( var);
		if (!idx) return defaultValue;
		return boost::lexical_cast<ValueType>( vscope.getValue( idx));
	}

	template <typename ValueType>
	ValueType get( const VariableScope& vscope, Variable::Id var) const
	{
		std::size_t idx = vscope.getValueIdx( var);
		if (!idx) throw_error( "required parameter not defined for this method", variableName( var));
		return boost::lexical_cast<ValueType>( vscope.getValue( idx));
	}

	template <typename ValueType>
	ValueType geti( const VariableScope& vscope, std::size_t idx) const
	{
		return boost::lexical_cast<ValueType>( vscope.getValue( idx));
	}

	template <typename ValueType>
	void set( VariableScope& vscope, Variable::Id var, ValueType& value)
	{
		vscope.define( var, boost::lexical_cast<std::string>(value));
	}

	const Page& page() const
	{
		if (m_pagear.empty()) throw_error( "no page for output defined", methodName(m_method));
		return m_pagear[ m_pageidx-1];
	}

	Page& page()
	{
		if (m_pagear.empty()) throw_error( "no page for output defined", methodName(m_method));
		return m_pagear[ m_pageidx-1];
	}

	HPDF_Page pagehnd() const
	{
		if (m_pagear.empty()) throw_error( "no page for output defined", methodName(m_method));
		return m_pagear[ m_pageidx-1].hnd();
	}

	struct VariableCache
	{
		std::size_t m_font_varidx;
		std::size_t m_fontsize_varidx;
		std::size_t m_linestyle_varidx;
		std::size_t m_positionX_varidx;
		std::size_t m_positionY_varidx;
		HPDF_REAL m_positionX;
		HPDF_REAL m_positionY;
		std::size_t m_sizeX_varidx;
		std::size_t m_sizeY_varidx;
		HPDF_REAL m_sizeX;
		HPDF_REAL m_sizeY;
		std::size_t m_align_varidx;
		Aligment m_align;

		VariableCache()
			:m_font_varidx(0)
			,m_fontsize_varidx(0)
			,m_linestyle_varidx(0)
			,m_positionX_varidx(0)
			,m_positionY_varidx(0)
			,m_sizeX_varidx(0)
			,m_sizeY_varidx(0)
			,m_sizeX(0.0)
			,m_sizeY(0.0)
			,m_align_varidx(0)
			,m_align(AlignLeft)
			{}

		void reset()
		{
			m_font_varidx = 0;
			m_fontsize_varidx = 0;
			m_linestyle_varidx = 0;
			m_positionX_varidx = 0;
			m_positionY_varidx = 0;
			m_sizeX_varidx = 0;
			m_sizeY_varidx = 0;
			m_sizeX = 0.0;
			m_sizeY = 0.0;
			m_align = AlignLeft;
			m_align_varidx = 0;
		}
	};

	std::string vardump( VariableScope& vscope)
	{
		std::ostringstream rt;

		VariableScope::const_iterator vi = vscope.begin(), ve = vscope.end();
		while (vi != ve)
		{
			if (vi != vscope.begin())
			{
				rt << ", ";
			}
			rt << variableName( vi->first) << " = '" << vscope.getValue( vi->second) << "'";
			++vi;
		}
		return rt.str();
	}

private:
	HPDF_Doc m_hnd;						//< libhpdf document handle
	std::vector<Page> m_pagear;				//< array of pages defined
	std::size_t m_pageidx;					//< page number starting from 1
	Method::Id m_method;					//< current method executed
	bool m_methodcall;					//< true, if we are currently executing a method
	std::map<std::size_t,Font> m_fontmap;			//< map of variable content references to font descriptions (constructed only once)
	std::map<std::size_t,LineStyle> m_linestylemap;		//< map of variable content references to line style (width,dash) descriptions (constructed only once)
	VariableCache m_vcache;					//< currently active object references set by variables of the caller
	std::vector< std::size_t> m_stk;			//< call counter stack
	std::size_t m_cnt;					//< call counter
};


struct DocumentImpl :public Document
{
	DocumentImpl(){}
	virtual ~DocumentImpl(){}

	virtual void execute_enter( Method::Id method, VariableScope& vscope)
	{
		m_impl.execute_enter( method, vscope);
	}
	virtual void execute_leave( Method::Id method, VariableScope& vscope)
	{
		m_impl.execute_leave( method, vscope);
	}

	virtual std::string tostring() const
	{
		return m_impl.tostring();
	}

private:
	HpdfDocument m_impl;
};
}// anonymous namespace


Document* _Wolframe::prnt::createLibHpdfDocument()
{
	return new DocumentImpl();
}


