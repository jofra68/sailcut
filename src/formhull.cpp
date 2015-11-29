/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "formhull.h"
#include "formhulldef.h"
#include "sailcpp/hullworker.h"

/**
 * Constructs a window to display a hull.
 *
 * @param parent the parent widget
 */
CFormHull::CFormHull(QWidget *parent)
    : CFormDocumentTmpl<CHullDef, CHullDefXmlWriter> (parent)
{
    setMinimumSize( QSize( 300, 220 ) );

    // create menu bar
    setupMenuBar();

    // create main widget
    QGridLayout *layout = new QGridLayout(this);
    tabs = new CSailViewerTabs(this);
    layout->addWidget(tabs);

    // set language
    connect(qApp, SIGNAL(languageChanged()),
            this, SLOT(languageChange()));
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormHull::languageChange()
{
    setWindowTitle( tr("hull") );
    actionViewDef->setText( tr("&Dimensions") );
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormHull::setDef(const CHullDef& newdef)
{
    def = newdef;
    tabs->setObject(CHullWorker(def).makeHull());
}


/**
 * Creates the menu bar
 */
void CFormHull::setupMenuBar()
{
    // view actions
    actionViewDef = new QAction(this);
    connect( actionViewDef, SIGNAL( triggered() ), this, SLOT( slotDef() ) );
    extraViewActions.push_back(actionViewDef);
}


/**
 * Displays the sail CFormHullDef sail definition dialog.
 */
void CFormHull::slotDef()
{
    CHullDef defcopy = def;

    if ( CFormHullDef(this, &defcopy).exec() )
    {
        // we returned from the dialog with an 'OK',
        setDef(defcopy);
    }
}

