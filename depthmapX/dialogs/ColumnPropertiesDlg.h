#// Copyright (C) 2011-2012, Tasos Varoudis

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "ui_ColumnPropertiesDlg.h"
#include <salalib/mgraph.h>
#include <salalib/attributes.h>
#include <salalib/shapemap.h>
#include <salalib/axialmap.h>

class CColumnPropertiesDlg : public QDialog, public Ui::CColumnPropertiesDlg
{
	Q_OBJECT
public:
    CColumnPropertiesDlg(dXreimpl::AttributeTable *table = NULL, LayerManagerImpl *layers = NULL, int col = -1, QWidget *parent = 0);
    dXreimpl::AttributeTable *m_table;
    LayerManagerImpl *m_layers;
	int		m_col;
	QString	m_formula;
	QString	m_name;
	QString	m_name_text;
	QString	m_creator;
	QString	m_formula_note;
	void UpdateData(bool value);
	void showEvent(QShowEvent * event);

private slots:
		void OnOK();
};
