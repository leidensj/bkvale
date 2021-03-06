#include "storeemployeestable.h"
#include <QHeaderView>
#include "tableitems/sqlitem.h"

StoreEmployeesTable::StoreEmployeesTable(JAddRemoveButtons* btns, QWidget* parent)
  : JTable(btns, parent)
{
  setColumnCount(1);
  QStringList headers;
  headers << "Funcionário";
  setHorizontalHeaderLabels(headers);

  horizontalHeader()->setSectionResizeMode((int)Column::Employee, QHeaderView::Stretch);
  connect(this, SIGNAL(changedSignal(bool)), this, SLOT(updateFilter()));
}

void StoreEmployeesTable::addRow()
{
  insertRow(rowCount());
  int row = rowCount() - 1;

  auto it = new SQLItem(EMPLOYEE_SQL_TABLE_NAME);

  blockSignals(true);
  setItem(row, (int)Column::Employee, it);
  blockSignals(false);

  setCurrentItem(it);
  setFocus();
}

void StoreEmployeesTable::addRowAndActivate()
{
  addRow();
  updateFilter();
  JTableItem* it = getItem(rowCount() - 1, (int)Column::Employee);
  it->activate();
  if (!SQLItem::st_idFromVariant(it->getValue()).isValid())
    removeItem();
}

void StoreEmployeesTable::getEmployees(QVector<Employee>& v) const
{
  v.clear();
  for (int i = 0; i != rowCount(); ++i)
  {
    int row = verticalHeader()->logicalIndex(i);
    Employee o;
    o.m_id = SQLItem::st_idFromVariant(getItem(row, (int)Column::Employee)->getValue());
    v.push_back(o);
  }
}

void StoreEmployeesTable::setEmployees(const QVector<Employee>& v)
{
  removeAllItems();
  for (int i = 0; i != v.size(); ++i)
  {
    addRow();
    getItem(i, (int)Column::Employee)->setValue(SQLItem::st_toVariant(v.at(i)));
  }
}

void StoreEmployeesTable::updateFilter()
{
  QString filter = EMPLOYEE_SQL_TABLE_NAME "." SQL_COLID " NOT IN ("
                   "SELECT " STORE_EMPLOYEES_SQL_TABLE_NAME "." STORE_EMPLOYEES_SQL_COL_EID
                   " FROM " STORE_EMPLOYEES_SQL_TABLE_NAME ")";
  if (rowCount() != 0)
  {
    filter += " AND " EMPLOYEE_SQL_TABLE_NAME "." SQL_COLID " NOT IN (";
    for (int i = 0; i != rowCount(); ++i)
      filter += SQLItem::st_idFromVariant(getItem(i, (int)Column::Employee)->getValue()).str() + ",";
    filter.chop(1);
    filter += ")";
  }
  for (int i = 0; i != rowCount(); ++i)
    dynamic_cast<SQLItem*>(getItem(i, (int)Column::Employee))->setFilter(filter);
}
