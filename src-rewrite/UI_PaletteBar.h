#ifndef UI_PALLETEBAR_H
#define UI_PALLETEBAR_H

/* This file is part of Step, modified to work on Rocs.
   Copyright (C) 2007 Vladimir Kuznetsov <ks.vladimir@gmail.com> (Original Author)
   Tomaz Canabrava - Rocs Modifications

   Step and Rocs are free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Step and Rocs are distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Step; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <QDockWidget>
#include <QList>

class QVBoxLayout;
class QToolButton;
class QScrollArea;
class QAction;
class QActionGroup;
class PaletteLayout;
class GraphScene;
class AbstractAction;

/*! \brief this class holds all the buttons of the pallete.
this is a container class for use with AbstractAction classes that will create
a item for each AbstractAction instance on the menu.
*/
class PaletteBar : public QDockWidget
{
	Q_OBJECT
	public:
		/*! 	default constructor 
			\param parent the MainWindow
			\param flags the window flags.
		*/
		PaletteBar ( QWidget* parent = 0, Qt::WindowFlags flags = 0 );
	
		/*! 
			whenever the active graph scene is changed, this method must be invocked 
			to atualize the links between the buttons and the scene.
			\param graphScene the new Scene.
		*/
		
		void changeActiveGraph(GraphScene* graphScene);

	protected slots:
		/*! shows or hides the text on the buttons
		\param b if true, shows the buttons, if false hides it. */
		void showButtonTextToggled ( bool b );

	protected:
		
		/*! creates a toolbutton and places it on the pallete.
		\param action the action of with the button will take it's functionalies. */

		void createToolButton ( AbstractAction* action );


		bool event ( QEvent* event );

		QScrollArea*    _scrollArea;
		QWidget*        _widget;
		PaletteLayout*  _layout;

		QAction*        _pointerAction;
		QList<AbstractAction*> _actionGroup;
		QList<QToolButton*> _toolButtons;
		int _numOfButtons;
};

#endif
