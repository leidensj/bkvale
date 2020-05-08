#include "databaseviewer.h"
#include "widgets/jlineedit.h"
#include "defines.h"
#include "pincodedialog.h"
#include "items/jitemex.h"
#include "models/jmodel.h"
#include <QDate>
#include <QLayout>
#include <QSqlRecord>
#include <QPushButton>
#include <QHeaderView>
#include <QShortcut>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QPixmap>
#include <QIcon>
#include <QCheckBox>
#include <QLabel>
#include <QSortFilterProxyModel>

JEnterSignalTable::JEnterSignalTable(QWidget *parent)
  : QTableView(parent)
{

}

void JEnterSignalTable::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Enter ||
      event->key() == Qt::Key_Return)
  {
    emit enterKeyPressedSignal();
  }
  QTableView::keyPressEvent(event);
}

DatabaseViewer::DatabaseViewer(const QString& tableName,
                               Mode mode,
                               QWidget *parent)
  : QWidget(parent)
  , m_mode(mode)
  , m_btnOpen(nullptr)
  , m_btnRefresh(nullptr)
  , m_btnRemove(nullptr)
  , m_ltButton(nullptr)
  , m_edSearch(nullptr)
  , m_cbContains(nullptr)
  , m_table(nullptr)
  , m_proxyModel(nullptr)
{
  m_btnOpen = new QPushButton();
  m_btnOpen->setFlat(true);
  m_btnOpen->setText("");
  m_btnOpen->setIconSize(QSize(24, 24));
  m_btnOpen->setIcon(QIcon(":/icons/res/open.png"));
  m_btnOpen->setToolTip(tr("Abrir (Ctrl+O)"));
  m_btnOpen->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

  m_btnRemove = new QPushButton();
  m_btnRemove->setFlat(true);
  m_btnRemove->setText("");
  m_btnRemove->setIconSize(QSize(24, 24));
  m_btnRemove->setToolTip(tr("Remover (Del)"));
  m_btnRemove->setIcon(QIcon(":/icons/res/remove.png"));
  m_btnRemove->setShortcut(QKeySequence(Qt::Key_Delete));

  m_btnRefresh = new QPushButton();
  m_btnRefresh->setFlat(true);
  m_btnRefresh->setText("");
  m_btnRefresh->setIconSize(QSize(16, 16));
  m_btnRefresh->setIcon(QIcon(":/icons/res/refresh.png"));
  m_btnRefresh->setToolTip(tr("Atualizar (F5)"));
  m_btnRefresh->setShortcut(QKeySequence(Qt::Key_F5));

  m_ltButton = new QHBoxLayout;
  m_ltButton->setContentsMargins(0, 0, 0, 0);
  m_ltButton->setAlignment(Qt::AlignLeft);
  m_ltButton->addWidget(m_btnOpen);
  m_ltButton->addWidget(m_btnRemove);

  m_edSearch = new JLineEdit(Text::Input::All, (int)JLineEdit::Flags::ToUpper);
  m_edSearch->setToolTip(tr("Procurar (Ctrl+F)"));
  m_edSearch->setClearButtonEnabled(true);

  m_cbContains = new QCheckBox;
  m_cbContains->setText(tr("Contendo"));
  m_cbContains->setIconSize(QSize(16, 16));
  m_cbContains->setIcon(QIcon(":/icons/res/center.png"));
  m_cbContains->setToolTip(tr("Procurar por palavras contendo o termo informado"));

  QHBoxLayout* hlayout1 = new QHBoxLayout();
  hlayout1->setContentsMargins(0, 0, 0, 0);
  hlayout1->addWidget(m_btnRefresh);
  hlayout1->addWidget(m_edSearch);
  hlayout1->addWidget(m_cbContains);

  m_table = new JEnterSignalTable;
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(m_mode == Mode::Full  ? QAbstractItemView::ExtendedSelection
                                                  : m_mode ==Mode::MultiSelector ? QAbstractItemView::MultiSelection
                                                                                 : QAbstractItemView::SingleSelection);
  m_table->setEditTriggers(QTableView::NoEditTriggers);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setHighlightSections(false);
  m_table->verticalHeader()->hide();

  QVBoxLayout* vlayout1 = new QVBoxLayout();
  vlayout1->addLayout(m_ltButton);
  vlayout1->addLayout(hlayout1);
  vlayout1->addWidget(m_table);
  setLayout(vlayout1);

  JModel* model = JItemEx::model(tableName, this);
  if (model == nullptr)
  {
    QMessageBox::critical(this, tr("Erro"), tr("Modelo não implementado"), QMessageBox::Ok);
    exit(EXIT_FAILURE);
  }

  m_proxyModel = new QSortFilterProxyModel(this);
  m_proxyModel->setSourceModel(model);
  m_proxyModel->setSortRole(Qt::EditRole);

  m_tableName = tableName;
  m_table->setModel(m_proxyModel);
  model->select(m_table->horizontalHeader());

  connect(m_table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(enableControls()));
  connect(m_table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(emitCurrentRowChangedSignal()));
  connect(m_btnRefresh, SIGNAL(clicked(bool)), this, SLOT(refresh()));
  connect(m_edSearch, SIGNAL(textEdited(const QString&)), this, SLOT(searchChanged()));
  connect(m_edSearch, SIGNAL(enterSignal()), this, SLOT(searchEnter()));
  connect(m_edSearch, SIGNAL(keyDownSignal()), this, SLOT(searchEnter()));
  connect(m_cbContains, SIGNAL(clicked(bool)), this, SLOT(containsPressed()));
  connect(m_table->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(searchChanged()));

  if (m_mode != Mode::ReadOnly)
  {
    connect(m_table, SIGNAL(enterKeyPressedSignal()), this, SLOT(emititemsSelectedSignalSignal()));
    connect(m_btnOpen, SIGNAL(clicked(bool)), this, SLOT(emititemsSelectedSignalSignal()));
    connect(m_table, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(emititemsSelectedSignalSignal()));
    connect(m_btnRemove, SIGNAL(clicked(bool)), this, SLOT(removeItems()));
  }

  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this, SLOT(focusSearch()));

  if (m_table->horizontalHeader()->count() > 1)
    m_table->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);

  if (m_mode == Mode::SingleSelector || m_mode == Mode::MultiSelector)
  {
    m_btnOpen->hide();
    m_btnRemove->setEnabled(false);
    m_btnRemove->hide();
    m_edSearch->setFocus();
  }
  else if (m_mode == Mode::ReadOnly)
  {
    m_btnOpen->hide();
    m_btnRemove->setEnabled(false);
    m_btnRemove->hide();
    m_edSearch->setFocus();
  }

  searchChanged();
}

DatabaseViewer::~DatabaseViewer()
{

}

Id DatabaseViewer::getFirstSelectedId() const
{
  QModelIndexList lst = m_table->selectionModel()->selectedRows();
  Id id;
  if (lst.size() != 0)
  {
    QModelIndex idx = m_proxyModel->mapToSource(lst.at(0));
    if (idx.isValid())
    {
      JModel* model = dynamic_cast<JModel*>(m_proxyModel->sourceModel());
      id = Id(model->index(idx.row(), SQL_COLID_NUMBER).data(Qt::EditRole).toLongLong());
    }
  }
  return id;
}

QVector<Id> DatabaseViewer::getSelectedIds() const
{
  JModel* model = dynamic_cast<JModel*>(m_proxyModel->sourceModel());
  QModelIndexList lst = m_table->selectionModel()->selectedRows();
  QVector<Id> ids;
  for (int i = 0; i != lst.size(); ++i)
  {
    QModelIndex idx = m_proxyModel->mapToSource(lst.at(i));
    if (idx.isValid())
      ids.push_back(Id(model->index(idx.row(), SQL_COLID_NUMBER).data(Qt::EditRole).toLongLong()));
  }
  return ids;
}

void DatabaseViewer::selectIds(const QVector<Id>& ids)
{
  for (int i = 0; i != ids.size(); ++i)
  {
    QModelIndexList lst = m_proxyModel->match(m_proxyModel->index(0, 0), Qt::EditRole, ids.at(i).get(), 1, Qt::MatchExactly);
    if (lst.size() != 0)
      m_table->selectRow(lst.at(0).row());
  }

  if (ids.size() != 0)
    m_table->setFocus();
}

void DatabaseViewer::refresh()
{
  JModel* model = dynamic_cast<JModel*>(m_proxyModel->sourceModel());
  if (m_fixedFilter.isEmpty() && m_dynamicFilter.isEmpty())
    model->select();
  else
  {
    QString str;
    if (!m_fixedFilter.isEmpty())
    {
      if (!m_dynamicFilter.isEmpty())
        str = m_fixedFilter + " AND " + m_dynamicFilter;
      else
        str = m_fixedFilter;
    }
    else
      str = m_dynamicFilter;
    model->selectFilter(str);
  }

  m_proxyModel->invalidate();
  enableControls();
  if (m_mode != Mode::Full)
    m_edSearch->setFocus();

  emit refreshSignal();
}

void DatabaseViewer::enableControls()
{
  bool bSelected = m_table->currentIndex().isValid();
  m_btnOpen->setEnabled(bSelected);
  m_btnRemove->setEnabled(bSelected);
}

void DatabaseViewer::removeItems()
{
  QVector<Id> ids = getSelectedIds();

  if (ids.size() == 0)
    return;

  if (QMessageBox::question(this,
                            tr("Remover itens"),
                            tr("Tem certeza que deseja remover os itens selecionados?"),
                            QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
  {
    return;
  }

  if (JItemEx::authenticationToRemove(m_tableName))
  {
    PinCodeDialog w(this);
    if (!w.exec())
      return;

    Employee e = w.getEmployee();
    QString error;
    if (!e.m_id.isValid())
      error = tr("Pincode informado não encontrado.");
    else if (!e.hasPermissionToRemove(m_tableName))
      error = tr("Funcionário não possui permissão.");

    if (!error.isEmpty())
    {
      QMessageBox::warning(this, tr("Erro"), error, QMessageBox::Ok);
      return;
    }
  }

  for (int i = 0; i != ids.size(); ++i)
  {
    Id id = ids.at(i);
    auto p = JItemEx::create(m_tableName, id);
    if (p != nullptr)
    {
      QString error;
      p->SQL_remove(error);
      delete p;
    }
  }

  emit itemsRemovedSignal(ids);
  refresh();
}

void DatabaseViewer::searchChanged()
{
  int column = m_table->horizontalHeader()->sortIndicatorSection();
  m_proxyModel->setFilterKeyColumn(column);
  QString search = m_edSearch->text();
  if (!m_cbContains->isChecked() && !search.isEmpty())
    search.prepend("\\b");
  QRegExp regExp(search, Qt::CaseInsensitive, QRegExp::RegExp);
  m_proxyModel->setFilterRegExp(regExp);

  if (search.isEmpty())
  {
    QString columnName;
    if (column > 0)
      columnName = m_proxyModel->headerData(column, Qt::Horizontal).toString().toLower();
    columnName = tr("Procurar por: ") + columnName;
    m_edSearch->setPlaceholderText(columnName);
  }

  enableControls();
  emit refreshSignal();
}

void DatabaseViewer::searchEnter()
{
  m_table->setFocus();
  if (m_proxyModel->rowCount() != 0)
  {
    m_table->selectRow(0);
  }
  else
  {
    m_edSearch->setFocus();
    m_edSearch->selectAll();
  }
}

void DatabaseViewer::clearSearch()
{
  m_edSearch->clear();
  searchChanged();
}

void DatabaseViewer::containsPressed()
{
  searchChanged();
  m_edSearch->setFocus();
}

void DatabaseViewer::focusSearch()
{
  m_edSearch->selectAll();
  m_edSearch->setFocus();
}

void DatabaseViewer::setFixedFilter(const QString& fixedFilter)
{
  m_fixedFilter = fixedFilter;
  refresh();
}

void DatabaseViewer::setDynamicFilter(const QString& dynamicFilter)
{
  m_dynamicFilter = dynamicFilter;
  refresh();
}

QString DatabaseViewer::getTableName() const
{
  return m_tableName;
}

bool DatabaseViewer::save(const JItemSQL& o)
{
  if (JItemEx::authenticationToInsertUpdate(m_tableName))
  {
    PinCodeDialog w(this);
    if (!w.exec())
      return false;

    Employee e = w.getEmployee();
    QString error;
    if (!e.m_id.isValid())
      error = tr("Pincode informado não encontrado.");
    else if (!e.hasPermissionToEdit(m_tableName))
      error = tr("Funcionário não possui permissão.");

    if (!error.isEmpty())
    {
      QMessageBox::warning(this, tr("Erro"), error, QMessageBox::Ok);
      return false;
    }
    o.setEmployee(e);
  }

  QString error;
  bool bSuccess = false;
  bSuccess = o.SQL_insert_update(error);

  if (bSuccess)
    refresh();
  else
    QMessageBox::critical(this, tr("Erro"), tr("Erro '%1' ao salvar o item.").arg(error), QMessageBox::Ok);
  return bSuccess;
}

void DatabaseViewer::emititemsSelectedSignalSignal()
{
  emit itemsSelectedSignal();
}

void DatabaseViewer::emitCurrentRowChangedSignal()
{
  emit currentRowChangedSignal(m_table->currentIndex().row());
}

int DatabaseViewer::getNumberOfEntries() const
{
  return m_proxyModel->rowCount();
}

double DatabaseViewer::getSum(int column) const
{
  double sum = 0.0;
  for (int row = 0; row != m_proxyModel->rowCount(); ++row)
    sum += m_proxyModel->data(m_proxyModel->index(row, column), Qt::EditRole).toDouble();
  return sum;
}

QPushButton* DatabaseViewer::addButton(const QString& toolTip, const QIcon& icon, int shortcut)
{
  QPushButton* btn = new QPushButton;
  btn->setFlat(true);
  btn->setToolTip(toolTip);
  btn->setIconSize(QSize(24, 24));
  btn->setIcon(icon);
  if (shortcut)
    btn->setShortcut(QKeySequence(shortcut));
  m_ltButton->addWidget(btn);
  return btn;
}
