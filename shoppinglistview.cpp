#include "shoppinglistview.h"
#include <QCheckBox>
#include <QLayout>
#include "jdatabasepicker.h"
#include "shoppinglisttable.h"
#include "packageeditor.h"
#include "jlineedit.h"
#include "jspinbox.h"
#include <QSplitter>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>

namespace {
QPushButton* monthButtonFactory(int n)
{
  QPushButton* btn = new QPushButton;
  if (n >= 0 && n < 31)
  {
    btn = new QPushButton;
    btn->setFlat(true);
    btn->setIconSize(QSize(32, 32));
    btn->setCheckable(true);
    QString strIcon = ":/icons/res/cal" + QString::number(n + 1) + ".png";
    btn->setIcon(QIcon(strIcon));
  }
  return btn;
}

QPushButton* weekButtonFactory(int n)
{
  QPushButton* btn = new QPushButton;
  if (n >= 0 && n < 7)
  {
    btn = new QPushButton;
    btn->setFlat(true);
    btn->setIconSize(QSize(32, 32));
    btn->setCheckable(true);
    QString strIcon = ":/icons/res/calw" + QString::number(n + 1) + ".png";
    btn->setIcon(QIcon(strIcon));
  }
  return btn;
}
}

ShoppingListView::ShoppingListView(QWidget* parent)
  : JItemView(SHOPPING_LIST_SQL_TABLE_NAME, parent)
  , m_btnAdd(nullptr)
  , m_btnRemove(nullptr)
  , m_edTitle(nullptr)
  , m_snLines(nullptr)
  , m_teDescription(nullptr)
  , m_supplierPicker(nullptr)
  , m_imagePicker(nullptr)
  , m_table(nullptr)
{
  m_btnAdd = new QPushButton;
  m_btnAdd->setFlat(true);
  m_btnAdd->setIconSize(QSize(24, 24));
  m_btnAdd->setIcon(QIcon(":/icons/res/additem.png"));
  m_btnAdd->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Plus));
  m_btnAdd->setToolTip(tr("Adicionar (Alt++)"));

  m_btnRemove = new QPushButton;
  m_btnRemove->setFlat(true);
  m_btnRemove->setIconSize(QSize(24, 24));
  m_btnRemove->setIcon(QIcon(":/icons/res/removeitem.png"));
  m_btnRemove->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Minus));
  m_btnRemove->setToolTip(tr("Remover (Alt+-)"));

  m_supplierPicker = new JDatabasePicker(PERSON_SQL_TABLE_NAME,
                                         tr("Fornecedor"),
                                         QIcon(":/icons/res/supplier.png"),
                                         true,
                                         true,
                                         false);
  m_imagePicker = new JDatabasePicker(IMAGE_SQL_TABLE_NAME,
                                      tr("Imagem"),
                                      QIcon(":/icons/res/icon.png"),
                                      true,
                                      true,
                                      false);
  m_edTitle = new JLineEdit(JLineEdit::Input::AlphanumericAndSpaces,
                            JLineEdit::st_defaultFlags1);

  m_snLines = new JSpinBox;
  m_snLines->setMinimum(0);
  m_snLines->setMaximum(30);

  m_teDescription = new QPlainTextEdit;

  for (int i = 0; i != 31; ++i)
    m_vbtnMonth[i] = monthButtonFactory(i);

  for (int i = 0; i != 7; ++i)
    m_vbtnWeek[i] = weekButtonFactory(i);

  QHBoxLayout* tableButtonLayout = new QHBoxLayout;
  tableButtonLayout->setAlignment(Qt::AlignLeft);
  tableButtonLayout->setContentsMargins(0, 0, 0, 0);
  tableButtonLayout->addWidget(m_btnAdd);
  tableButtonLayout->addWidget(m_btnRemove);

  QHBoxLayout* monthLayout1 = new QHBoxLayout;
  monthLayout1->setContentsMargins(0, 0, 0, 0);
  monthLayout1->setAlignment(Qt::AlignLeft);
  for (int i = 0; i != 7; ++i)
    monthLayout1->addWidget(m_vbtnMonth[i]);

  QHBoxLayout* monthLayout2 = new QHBoxLayout;
  monthLayout2->setContentsMargins(0, 0, 0, 0);
  monthLayout2->setAlignment(Qt::AlignLeft);
  for (int i = 7; i != 14; ++i)
    monthLayout2->addWidget(m_vbtnMonth[i]);

  QHBoxLayout* monthLayout3 = new QHBoxLayout;
  monthLayout3->setContentsMargins(0, 0, 0, 0);
  monthLayout3->setAlignment(Qt::AlignLeft);
  for (int i = 14; i != 21; ++i)
    monthLayout3->addWidget(m_vbtnMonth[i]);

  QHBoxLayout* monthLayout4 = new QHBoxLayout;
  monthLayout4->setContentsMargins(0, 0, 0, 0);
  monthLayout4->setAlignment(Qt::AlignLeft);
  for (int i = 21; i != 28; ++i)
    monthLayout4->addWidget(m_vbtnMonth[i]);

  QHBoxLayout* monthLayout5 = new QHBoxLayout;
  monthLayout5->setContentsMargins(0, 0, 0, 0);
  monthLayout5->setAlignment(Qt::AlignLeft);
  for (int i = 28; i != 31; ++i)
    monthLayout5->addWidget(m_vbtnMonth[i]);

  QHBoxLayout* weekLayout1 = new QHBoxLayout;
  weekLayout1->setContentsMargins(0, 0, 0, 0);
  weekLayout1->setAlignment(Qt::AlignLeft);
  for (int i = 0; i != 7; ++i)
    weekLayout1->addWidget(m_vbtnWeek[i]);

  QFormLayout* viewFormLayout = new QFormLayout;
  viewFormLayout->addRow(tr("Título:"), m_edTitle);
  viewFormLayout->addWidget(m_supplierPicker);
  viewFormLayout->addWidget(m_imagePicker);
  viewFormLayout->addRow(tr("Linhas:"), m_snLines);
  viewFormLayout->addRow(tr("Descrição:"), m_teDescription);

  QVBoxLayout* calendarLayout = new QVBoxLayout;
  calendarLayout->setAlignment(Qt::AlignTop);
  calendarLayout->addWidget(new QLabel(tr("Dias que a lista de compras deve ser preenchida:")));
  calendarLayout->addWidget(new QLabel(tr("Semanal:")));
  calendarLayout->addLayout(weekLayout1);
  calendarLayout->addWidget(new QLabel(tr("Mensal:")));
  calendarLayout->addLayout(monthLayout1);
  calendarLayout->addLayout(monthLayout2);
  calendarLayout->addLayout(monthLayout3);
  calendarLayout->addLayout(monthLayout4);
  calendarLayout->addLayout(monthLayout5);

  m_table = new ShoppingListTable;
  QVBoxLayout* listLayout = new QVBoxLayout;
  listLayout->addLayout(tableButtonLayout);
  listLayout->addWidget(m_table);

  QFrame* tabView = new QFrame;
  tabView->setLayout(viewFormLayout);

  QFrame* tabList = new QFrame;
  tabList->setLayout(listLayout);

  QFrame* tabCalendar = new QFrame;
  tabCalendar->setLayout(calendarLayout);

  m_tab->addTab(tabView,
                QIcon(":/icons/res/shopmgt.png"),
                tr("Lista de Compras"));
  m_tab->addTab(tabList,
                QIcon(":/icons/res/item.png"),
                tr("Produtos"));
  m_tab->addTab(tabCalendar,
                QIcon(":/icons/res/calendar.png"),
                tr("Calendário"));

  QObject::connect(m_btnAdd,
                   SIGNAL(clicked(bool)),
                   this,
                   SLOT(addItem()));
  QObject::connect(m_btnRemove,
                   SIGNAL(clicked(bool)),
                   this,
                   SLOT(removeItem()));
  QObject::connect(m_table,
                   SIGNAL(productSignal(const Product&)),
                   this,
                   SLOT(editProduct()));
  QObject::connect(m_table,
                   SIGNAL(packageSignal(const Package&,
                                        const QString&)),
                   this,
                   SLOT(editPackage(const Package&, const QString&)));
  QObject::connect(m_table,
                   SIGNAL(changedSignal()),
                   this,
                   SLOT(updateControls()));

  m_supplierPicker->getDatabase()->setFixedFilter(PERSON_FILTER_SUPPLIER);
}

void ShoppingListView::addItem()
{
  JDatabaseSelector w(PRODUCT_SQL_TABLE_NAME, tr("Produto"), QIcon(":/icons/res/item.png"), this);
  w.getDatabase()->setFixedFilter(PRODUCT_FILTER_SHOP);
  if (w.exec())
  {
    Product* p = static_cast<Product*>(w.getDatabase()->getCurrentItem());
    if (p != nullptr && p->isValidId())
    {
      ShoppingListItem o;
      o.m_product = *p;
      m_table->addShoppingItem(o);
    }
  }
}

void ShoppingListView::removeItem()
{
  m_table->removeCurrentItem();
  updateControls();
}

void ShoppingListView::editProduct()
{
  JDatabaseSelector w(PRODUCT_SQL_TABLE_NAME, tr("Produto"), QIcon(":/icons/res/item.png"), this);
  w.getDatabase()->setFixedFilter(PRODUCT_FILTER_SHOP);
  if (w.exec())
  {
    Product* p = static_cast<Product*>(w.getDatabase()->getCurrentItem());
    if (p != nullptr)
    {
      Product o = *p;
      m_table->setProduct(o);
    }
  }
}

void ShoppingListView::editPackage(const Package& package,
                                   const QString& productUnity)
{
  PackageEditor dlg(package, productUnity);
  if (dlg.exec())
    m_table->setPackage(dlg.getPackage());
}

void ShoppingListView::updateControls()
{
  m_btnAdd->setEnabled(m_table->rowCount() < SHOPPING_LIST_MAX_NUMBER_OF_ITEMS);
  m_btnRemove->setEnabled(m_table->currentRow() != -1);
}

void ShoppingListView::create()
{
  selectItem(ShoppingList());
  m_tab->setCurrentIndex(0);
  m_edTitle->setFocus();
  updateControls();
}

void ShoppingListView::setItem(const JItem& o)
{
  auto _o = dynamic_cast<const ShoppingList&>(o);
  m_currentId = _o.m_id;
  m_edTitle->setText(_o.m_title);
  m_teDescription->setPlainText(_o.m_description);
  m_supplierPicker->setItem(_o.m_supplier);
  m_imagePicker->setItem(_o.m_image);
  m_snLines->setValue(_o.m_nLines);
  for (int i = 0; i != 7; ++i)
    m_vbtnWeek[i]->setChecked(_o.m_weekDays[i]);
  for (int i = 0; i != 31; ++i)
    m_vbtnMonth[i]->setChecked(_o.m_monthDays[i]);
  m_table->setShoppingItems(_o.m_vItem);
  updateControls();
}

const JItem& ShoppingListView::getItem() const
{
  static ShoppingList o;
  o.m_id = m_currentId;
  o.m_title = m_edTitle->text();
  o.m_description = m_teDescription->toPlainText();
  o.m_supplier.m_id = m_supplierPicker->getId();
  o.m_image.m_id = m_imagePicker->getId();
  o.m_nLines = m_snLines->value();
  for (int i = 0; i != 7; ++i)
    o.m_weekDays[i] = m_vbtnWeek[i]->isChecked();
  for (int i = 0; i != 31; ++i)
    o.m_monthDays[i] = m_vbtnMonth[i]->isChecked();
  o.m_vItem = m_table->getShoppingItems();
  return o;
}
