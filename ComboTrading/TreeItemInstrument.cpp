/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// started December 13, 2015, 8:16 PM

// 20161003 
//  manual control of watch/unwatch in menu at some point
//  set colour on menu item for watch/unwatch mode
//  then start the watch chart
//  then provide shift/zoom capability on the chart

// 20161009
//   should override rename so it always has proper instrument name
//   is rename actually in the menu, if not, then nothing to do

#include <wx-3.0/wx/menu.h>

#include "TreeItemInstrument.h"

TreeItemInstrument::TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
  TreeItemResources( id, baseResources, resources ), m_lockType( InstrumentActions::ENewInstrumentLock::NoLock ) {
  m_pInstrumentActions = m_resources.signalGetInstrumentActions( m_id );
  assert( 0 != m_pInstrumentActions.use_count() );
}
  
TreeItemInstrument::~TreeItemInstrument( void ) {
  m_pInstrumentActions->signalDelete( m_id );
}

void TreeItemInstrument::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Delete: TreeItemInstrument" << std::endl;
  //ou::tf::TreeItemBase::DeleteMember( m_id );
  m_baseResources.signalDelete( m_id );
}

void TreeItemInstrument::HandleLiveChart( wxCommandEvent& event ) {
  HandleOnClick();
}

void TreeItemInstrument::HandleOnClick( void ) {
  m_pInstrumentActions->signalLiveChart( m_id );
}

void TreeItemInstrument::HandleMenuOptionList( wxCommandEvent& event ) {
  m_pInstrumentActions->signalOptionList( m_id );
}

void TreeItemInstrument::HandleDailyChart( wxCommandEvent& event ) {
  m_pInstrumentActions->signalDailyChart( m_id );
}

void TreeItemInstrument::HandleEmit( wxCommandEvent& event ) {
  m_pInstrumentActions->signalEmitValues( m_id );
}

//void TreeItemInstrument::HandleSaveData( wxCommandEvent& event ) {
//  m_pInstrumentActions->signalSaveData( m_id );
//}

void TreeItemInstrument::BuildContextMenu( wxMenu* pMenu ) {
  assert( 0 != pMenu );
  if ( InstrumentActions::ENewInstrumentLock::NoLock == m_lockType ) {  // is this actually used?
    pMenu->Append( MINewInstrument, "New Instrument" );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuNewInstrument, this, MINewInstrument );
  }
  else {
    pMenu->Append( MIOptionList, "Option List" );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuOptionList, this, MIOptionList );
    if ( InstrumentActions::ENewInstrumentLock::LockFuturesOption == m_lockType ) {
      // can then use underlying to calc implied volatility
      pMenu->Append( MINewFuturesOption, "New Futures Option" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuAddFuturesOption, this, MINewFuturesOption );
    }
    else {
      if ( InstrumentActions::ENewInstrumentLock::LockOption == m_lockType ) {
      // can then use underlying to calc implied volatility
        pMenu->Append( MINewOption, "New Option" );
        pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuAddOption, this, MINewOption );
      }
      else {
        std::cout << "TreeItemInstrument::BuildContextMenu has unknown lockType: " << m_lockType << std::endl;
      }
    }
  }
  // add watch/unwatch menu item?
  pMenu->Append( MIEmit, "Emit" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleEmit, this, MIEmit );
  pMenu->Append( MILiveChart, "Live Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleLiveChart, this, MILiveChart );
  pMenu->Append( MIDailyChart, "Daily Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDailyChart, this, MIDailyChart );
//  pMenu->Append( MISaveData, "Save Data" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleSaveData, this, MISaveData );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDelete, this, MIDelete );
}

void TreeItemInstrument::ShowContextMenu( void ) {
  if ( 0 == m_pMenu ) {
    m_pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
    TreeItemInstrument::BuildContextMenu( m_pMenu );
  }
  m_baseResources.signalPopupMenu( m_pMenu );
}

/* todo:  20170428 mostly complete
 *   for following two handlers:
 *   the lock should be forcing the gui to show options only for the underlying instrument
 *     when adding sub-menus to the tree
 *   the dialog needs a lock for FuturesOption and Option
 *   then need NewOptionViaDialog, NewFuturesOptionViaDialog to force that setting in the dialog
 */

// from tree menu popup
void TreeItemInstrument::HandleMenuAddOption( wxCommandEvent& event ) { 
  InstrumentViaDialog( InstrumentActions::LockOption, "Option" );
}

// from tree menu popup
void TreeItemInstrument::HandleMenuAddFuturesOption( wxCommandEvent& event ) { 
  InstrumentViaDialog( InstrumentActions::LockFuturesOption, "FuturesOption" );
}

void TreeItemInstrument::HandleMenuNewInstrument( wxCommandEvent& event ) {
  InstrumentViaDialog( InstrumentActions::NoLock, "Instrument Name" );
}

void TreeItemInstrument::InstrumentViaDialog( InstrumentActions::ENewInstrumentLock lock, const std::string& sPrompt ) {
  TreeItemInstrument* p = AddTreeItem<TreeItemInstrument>( sPrompt, IdInstrument, m_resources );
  wxString wsx( m_baseResources.signalGetItemText( m_id ) );
  if ( p->NewInstrumentViaDialog( lock, wsx ) ) {  // wsx is used as sUnderlying to new instrument, when needed
    // continue with processing
  }
  else {
    // delete the menuitem if nothing chosen
    wxTreeItemId id( p->GetTreeItemId() );
    m_baseResources.signalDelete( id );
    ou::tf::TreeItemBase::DeleteMember( id );
  }
}

// called by: 
//   InstrumentViaDialog (above)
//   TreeItemGroup::HandleAddInstrument
bool TreeItemInstrument::NewInstrumentViaDialog( 
  InstrumentActions::ENewInstrumentLock lock, const wxString& wxsUnderlying 
  ) {
  // IQF underlying name, and IB underlying name will need to be provided for naming options during lock
  // need to assume/assert that this is a new dialog?  or communicate it is a replacement?
  bool bInstrumentNameAssigned( false );
  InstrumentActions::values_t values( 
    m_pInstrumentActions->signalNewInstrument( this->m_id, lock, wxsUnderlying ) );
  if ( "" == values.name_ ) {
    // caller takes care of the deletion
  }
  else {
    m_baseResources.signalSetItemText( m_id, values.name_ );
    if ( !wxsUnderlying.empty() ) 
      m_sUnderlying = wxsUnderlying;
    m_lockType = values.lockType_;
    bInstrumentNameAssigned = true;
  }
  return bInstrumentNameAssigned;
}
