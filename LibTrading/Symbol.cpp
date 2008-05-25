#include "StdAfx.h"
#include "Symbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSymbol::CSymbol( const std::string sName ): 
  m_sSymbolName( sName )
{
}

CSymbol::~CSymbol(void) {
}

bool CSymbol::AddQuoteHandler(quotehandler_t handler) {
  Delegate<quote_t>::vsize_t size = m_OnQuote.Size();
  m_OnQuote.Add( handler );
  assert( size == ( m_OnQuote.Size() - 1 ) );
  return ( 1 == m_OnQuote.Size() );  // start watch for the symbol
}

bool CSymbol::RemoveQuoteHandler(quotehandler_t handler) {
  assert( 0 < m_OnQuote.Size() );
  m_OnQuote.Remove( handler );
  return ( 0 == m_OnQuote.Size() );  // no more so stop watch
}

bool CSymbol::AddTradeHandler(tradehandler_t handler) {
  Delegate<trade_t>::vsize_t size = m_OnTrade.Size();
  m_OnTrade.Add( handler );
  assert( size == ( m_OnTrade.Size() - 1 ) );
  return ( 1 == m_OnTrade.Size() ); // start watch on first handler
}

bool CSymbol::RemoveTradeHandler(tradehandler_t handler) {
  assert( 0 < m_OnTrade.Size() );
  m_OnTrade.Remove( handler );
  return ( 0 == m_OnTrade.Size() ); // no more so stop watch
}

bool CSymbol::AddDepthHandler(depthhandler_t handler) {
  Delegate<depth_t>::vsize_t size = m_OnDepth.Size();
  m_OnDepth.Add( handler );
  assert( size == ( m_OnDepth.Size() - 1 ) );
  return ( 1 == m_OnDepth.Size() );  // when true, start watch
}

bool CSymbol::RemoveDepthHandler(depthhandler_t handler) {
  assert( 0 < m_OnDepth.Size() );
  m_OnDepth.Remove( handler );
  return ( 0 == m_OnDepth.Size() );  // when true, stop watch
}


/*
What is effectively happening, is that myFoo is being "bound" into the newFunc object. 
Think of it as creating a private variable inside newFunc and sticking myFoo in it. 
When newFunc is invoked, it will use myFoo as a parameter. 
http://orionedwards.blogspot.com/2006/09/function-pointers-in-cc-and-boostbind.html
*/


/*
class Button {
  typedef boost::signal<void (int x, int y)> OnClick;

public:
  void doOnClick(const OnClick::slot_type& slot);

private:
  OnClick onClick;
};

void Button::doOnClick( const OnClick::slot_type& slot ) {
  onClick.connect(slot);
}

void printCoordinates(long x, long y) {
  std::cout << "(" << x << ", " << y << ")\n";
}

void f(Button& button) {
  button.doOnClick(&printCoordinates);
}
*/