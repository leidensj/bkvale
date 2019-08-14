#include "purchaseview.h"
#include "databaseutils.h"
#include "tables/purchasetable.h"
#include "widgets/jdatabasepicker.h"
#include "widgets/jdatabase.h"
#include "widgets/jlineedit.h"
#include "widgets/jaddremovebuttons.h"
#include "widgets/jdoublespinbox.h"
#include "widgets/jtable.h"
#include "widgets/jtablewidgetitem.h"
#include "widgets/jdatepicker.h"
#include "filters/purchasefilter.h"
#include "widgets/jdatabasecombobox.h"
#include "packageeditor.h"
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QTabWidget>
#include <QFormLayout>
#include <QRadioButton>
#include "items/jitemex.h"

PaymentWidget::PaymentWidget(QWidget* parent)
  : QWidget(parent)
  , m_rdoCash(nullptr)
  , m_rdoBonus(nullptr)
  , m_rdoCredit(nullptr)
  , m_tbCredit(nullptr)
  , m_btnAddRemove(nullptr)
  , m_purchaseTotal(0.0)
  , m_purchaseDate(QDate::currentDate())
{
  m_rdoCredit = new QRadioButton;
  m_rdoCredit->setIcon(QIcon(":/icons/res/credit.png"));
  m_rdoCredit->setText(tr("A prazo"));

  m_rdoCash = new QRadioButton;
  m_rdoCash->setIcon(QIcon(":/icons/res/cash.png"));
  m_rdoCash->setText(tr("A vista"));

  m_rdoBonus = new QRadioButton;
  m_rdoBonus->setIcon(QIcon(":/icons/res/bonus.png"));
  m_rdoBonus->setText(tr("Bonificação"));

  m_lblPurchaseTotal = new QLabel;
  m_lblPaymentTotal = new QLabel;

  m_btnAddRemove = new JAddRemoveButtons;
  m_tbCredit = new JTable;
  m_tbCredit->setColumnCount(2);
  QStringList headers;
  headers << "Data" << "Valor";
  m_tbCredit->setHorizontalHeaderLabels(headers);
  m_tbCredit->horizontalHeader()->setSectionResizeMode((int)Column::Date, QHeaderView::ResizeMode::ResizeToContents);
  m_tbCredit->horizontalHeader()->setSectionResizeMode((int)Column::Value, QHeaderView::ResizeMode::Stretch);

  QHBoxLayout* ltButtons = new QHBoxLayout;
  ltButtons->setAlignment(Qt::AlignLeft);
  ltButtons->addWidget(m_rdoCredit);
  ltButtons->addWidget(m_rdoCash);
  ltButtons->addWidget(m_rdoBonus);

  QVBoxLayout* lt = new QVBoxLayout;
  lt->addLayout(ltButtons);
  lt->addWidget(m_btnAddRemove);
  lt->addWidget(m_tbCredit);
  lt->addWidget(m_lblPurchaseTotal);
  lt->addWidget(m_lblPaymentTotal);

  connect(m_rdoCash, SIGNAL(clicked(bool)), this, SLOT(updateControls()));
  connect(m_rdoBonus, SIGNAL(clicked(bool)), this, SLOT(updateControls()));
  connect(m_rdoCash, SIGNAL(clicked(bool)), this, SLOT(emitMethodChangedSignal()));
  connect(m_rdoBonus, SIGNAL(clicked(bool)), this, SLOT(emitMethodChangedSignal()));
  connect(m_rdoCredit, SIGNAL(clicked(bool)), this, SLOT(emitMethodChangedSignal()));
  connect(m_rdoCash, SIGNAL(clicked(bool)), m_tbCredit, SLOT(removeAllItems()));
  connect(m_rdoBonus, SIGNAL(clicked(bool)), m_tbCredit, SLOT(removeAllItems()));
  connect(m_rdoCredit, SIGNAL(clicked(bool)), this, SLOT(fillCredit()));
  connect(m_btnAddRemove->m_btnAdd, SIGNAL(clicked(bool)), this, SLOT(addRow()));
  connect(m_btnAddRemove->m_btnRemove, SIGNAL(clicked(bool)), this, SLOT(removeRow()));
  connect(m_tbCredit, SIGNAL(changedSignal(bool)), m_btnAddRemove->m_btnRemove, SLOT(setEnabled(bool)));
  connect(m_tbCredit, SIGNAL(changedSignal(bool)), this, SLOT(updateControls()));
  connect(m_tbCredit, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateTable(QTableWidgetItem*)));

  setLayout(lt);
  updateControls();
}

QIcon PaymentWidget::getIcon() const
{
  if (m_rdoBonus->isChecked())
    return QIcon(":/icons/res/bonus.png");
  if (m_rdoCash->isChecked())
    return QIcon(":/icons/res/cash.png");
  return QIcon(":/icons/res/credit.png");
}

QString PaymentWidget::getText() const
{
  if (m_rdoBonus->isChecked())
    return tr("A prazo");
  if (m_rdoCash->isChecked())
    return tr("A vista");
  return tr("Crédito");
}

bool PaymentWidget::isDatesValid() const
{
  bool bValid = true;
  if (m_rdoCredit->isChecked())
    for (int i = 0; i != m_tbCredit->rowCount() && bValid; ++i)
      bValid = dynamic_cast<DateItem*>(m_tbCredit->item(i, (int)Column::Date))->getDate() >= m_purchaseDate;
  return bValid;
}

double PaymentWidget::computeTotal() const
{
  double total = 0.0;
  if (m_rdoCredit->isChecked())
    for (int i = 0; i != m_tbCredit->rowCount(); ++i)
      total += dynamic_cast<DoubleItem*>(m_tbCredit->item(i, (int)Column::Value))->getValue();
  return total;
}

void PaymentWidget::updateControls()
{
  m_tbCredit->setEnabled(m_rdoCredit->isChecked());
  m_btnAddRemove->setEnabled(m_rdoCredit->isChecked());
  double total = computeTotal();
  bool bValid = isDatesValid() && JItem::st_areEqual(m_purchaseTotal, total, JItem::DataType::Money);;
  m_lblPurchaseTotal->setText(tr("Total da compra: ") + JItem::st_strMoney(m_purchaseTotal));
  m_lblPaymentTotal->setText(("Total do pagamento: ") + JItem::st_strMoney(total));
  m_lblPaymentTotal->setVisible(m_rdoCredit->isChecked());
  setWindowIcon(getIcon());
  emit isValidSignal(bValid);
}

void PaymentWidget::updateTable(QTableWidgetItem* p)
{
  dynamic_cast<ExpItem*>(p)->evaluate();
}

void PaymentWidget::fillCredit()
{
  m_tbCredit->removeAllItems();
  addRow();
  updateControls();
}

Purchase::PaymentMethod PaymentWidget::getPaymentMethod() const
{
  if (m_rdoBonus->isChecked())
    return Purchase::PaymentMethod::Bonus;
  if (m_rdoCash->isChecked())
    return Purchase::PaymentMethod::Cash;
  return Purchase::PaymentMethod::Credit;
}

QVector<PaymentElement> PaymentWidget::getPayments() const
{
  QVector<PaymentElement> v;
  if (m_rdoCredit->isChecked())
  {
    for (int i = 0; i != m_tbCredit->rowCount(); ++i)
    {
      PaymentElement o;
      o.m_value += dynamic_cast<DoubleItem*>(m_tbCredit->item(i, (int)Column::Value))->getValue();
      o.m_date = dynamic_cast<DateItem*>(m_tbCredit->item(i, (int)Column::Date))->getDate();
      v.push_back(o);
    }
  }
  return v;
}

void PaymentWidget::setPurchaseDate(const QDate& dt)
{
  m_purchaseDate = dt;
  updateControls();
}

void PaymentWidget::setPurchaseTotal(double value)
{
  m_purchaseTotal = value;
  updateControls();
}

void PaymentWidget::setPaymentMethod(Purchase::PaymentMethod o)
{
  if (o == Purchase::PaymentMethod::Bonus)
    m_rdoBonus->setChecked(true);
  else if (o == Purchase::PaymentMethod::Cash)
    m_rdoCash->setChecked(true);
  else
    m_rdoCredit->setChecked(true);
}

void PaymentWidget::setPayments(const QVector<PaymentElement>& v)
{
  m_tbCredit->removeAllItems();
  for (int i = 0; i != v.size(); ++i)
  {
    addRow();
    dynamic_cast<DoubleItem*>(m_tbCredit->item(i, (int)Column::Value))->setValue(v.at(i).m_value);
    dynamic_cast<DateItem*>(m_tbCredit->item(i, (int)Column::Date))->setDate(v.at(i).m_date);
  }
}

void PaymentWidget::addRow()
{
  m_tbCredit->insertRow(m_tbCredit->rowCount());
  int row = m_tbCredit->rowCount() - 1;

  auto itValue = new DoubleItem(JItem::DataType::Money, DoubleItem::Color::Foreground);
  auto itDate = new DateItem(m_purchaseDate, DateItem::Color::DateBeforeDefault);

  m_tbCredit->setItem(row, (int)Column::Date, itDate);
  m_tbCredit->setItem(row, (int)Column::Value, itValue);

  double total = computeTotal();
  double val = m_purchaseTotal > total ? m_purchaseTotal - total : 0.0;
  itValue->setValue(val);
  itDate->setDate(m_purchaseDate.addMonths(itDate->row() + 1));
  updateControls();
  m_tbCredit->setCurrentItem(itDate);
  m_tbCredit->setFocus();
}

void PaymentWidget::removeRow()
{
  if (m_tbCredit->currentRow() >= 0)
    m_tbCredit->removeRow(m_tbCredit->currentRow());
  updateControls();
}

void PaymentWidget::emitMethodChangedSignal()
{
  emit methodChangedSignal();
}

PurchaseView::PurchaseView(QWidget *parent)
  : JItemView(PURCHASE_SQL_TABLE_NAME, parent)
  , m_btnOpenLast(nullptr)
  , m_btnAddCode(nullptr)
  , m_btnAddRemove(nullptr)
  , m_snNumber(nullptr)
  , m_dtPicker(nullptr)
  , m_edTotal(nullptr)
  , m_supplierPicker(nullptr)
  , m_table(nullptr)
  , m_edDisccount(nullptr)
  , m_wPayment(nullptr)
  , m_edEntries(nullptr)
  , m_edSum(nullptr)
  , m_teObservation(nullptr)
  , m_filter(nullptr)
  , m_btnApportionment(nullptr)
  , m_cbStore(nullptr)
{
  m_btnSave->setEnabled(false);
  m_btnSave->hide();

  m_btnOpenLast = new QPushButton;
  m_btnOpenLast->setFlat(true);
  m_btnOpenLast->setText("");
  m_btnOpenLast->setIconSize(QSize(24, 24));
  m_btnOpenLast->setIcon(QIcon(":/icons/res/openlast.png"));
  m_btnOpenLast->setToolTip(tr("Abrir último item"));

  m_ltButton->addWidget(m_btnOpenLast);

  m_btnAddCode = new QPushButton;
  m_btnAddCode->setFlat(true);
  m_btnAddCode->setText("");
  m_btnAddCode->setIconSize(QSize(24, 24));
  m_btnAddCode->setIcon(QIcon(":/icons/res/barcodescan.png"));
  m_btnAddCode->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Asterisk));
  m_btnAddCode->setToolTip(tr("Adicionar pelo código(Alt+*)"));

  m_btnAddRemove = new JAddRemoveButtons;
  m_btnAddRemove->m_lt->addWidget(m_btnAddCode);

  QLabel* lblNumber = new QLabel();
  lblNumber->setText(tr("Número:"));
  {
    QFont font = lblNumber->font();
    font.setPointSize(12);
    lblNumber->setFont(font);
  }

  m_cbStore = new JDatabaseComboBox(STORE_SQL_TABLE_NAME, 1);

  m_snNumber = new QSpinBox();
  m_snNumber->setReadOnly(true);
  m_snNumber->setButtonSymbols(QSpinBox::ButtonSymbols::NoButtons);
  m_snNumber->setMaximum(99999999);
  m_snNumber->setMinimum(0);
  m_snNumber->setSpecialValueText(tr("S/N"));
  m_snNumber->setMinimumSize(90, 0);
  m_snNumber->setAlignment(Qt::AlignRight);
  m_snNumber->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
  {
    QFont font = m_snNumber->font();
    font.setBold(true);
    m_snNumber->setFont(font);
    QPalette palette = m_snNumber->palette();
    palette.setColor(QPalette::ColorRole::Text, Qt::red);
    m_snNumber->setPalette(palette);
  }

  m_dtPicker = new JDatePicker;

  QLabel* lblDate = new QLabel();
  lblDate->setText(tr("Data:"));
  {
    QFont font = lblDate->font();
    font.setPointSize(12);
    lblDate->setFont(font);
  }

  QFrame* line1 = new QFrame;
  line1->setFrameShape(QFrame::VLine);

  QFrame* line2 = new QFrame;
  line2->setFrameShape(QFrame::VLine);

  QHBoxLayout* ltCmd = new QHBoxLayout();
  ltCmd->setContentsMargins(0, 0, 0, 0);
  ltCmd->setAlignment(Qt::AlignLeft);
  ltCmd->addWidget(lblNumber);
  ltCmd->addWidget(m_snNumber);
  ltCmd->addWidget(line1);
  ltCmd->addWidget(lblDate);
  ltCmd->addWidget(m_dtPicker);
  ltCmd->addWidget(line2);
  ltCmd->addWidget(m_btnAddRemove);

  m_supplierPicker = new JDatabasePicker(SUPPLIER_SQL_TABLE_NAME);
  m_supplierPicker->setPlaceholderText(true);

  QVBoxLayout* ltHeader = new QVBoxLayout;
  ltHeader->addWidget(m_cbStore);
  ltHeader->addLayout(ltCmd);
  ltHeader->addWidget(m_supplierPicker);

  QFrame* frHeader = new QFrame();
  frHeader->setFrameShape(QFrame::Shape::StyledPanel);
  frHeader->setFrameShadow(QFrame::Shadow::Plain);
  frHeader->setLayout(ltHeader);
  frHeader->setFixedHeight(frHeader->sizeHint().height());

  m_table = new PurchaseTable;

  m_edTotal = new JExpLineEdit(JItem::DataType::Money);
  m_edTotal->setReadOnly(true);
  m_edTotal->setPlaceholderText(tr("TOTAL"));
  m_edTotal->setAlignment(Qt::AlignRight);
  m_edTotal->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  {
    QFont font = m_edTotal->font();
    font.setBold(true);
    m_edTotal->setFont(font);
  }

  m_edDisccount = new JExpLineEdit(JItem::DataType::Money);
  m_edDisccount->setAlignment(Qt::AlignRight);
  m_edDisccount->setPlaceholderText(tr("Descontos ou acréscimos"));

  m_btnApportionment = new QPushButton;
  m_btnApportionment->setFlat(true);
  m_btnApportionment->setIconSize(QSize(16, 16));
  m_btnApportionment->setIcon(QIcon(":/icons/res/apportionment.png"));
  m_btnApportionment->setToolTip(tr("Ratear valor entre os produtos"));

  QHBoxLayout* ltTotal = new QHBoxLayout;
  ltTotal->setContentsMargins(0, 0, 0, 0);
  ltTotal->addWidget(m_edDisccount);
  ltTotal->addWidget(m_btnApportionment);
  ltTotal->addStretch();
  ltTotal->addWidget(m_edTotal);

  QVBoxLayout* ltInfo = new QVBoxLayout;
  ltInfo->addWidget(frHeader);
  ltInfo->addWidget(m_table);
  ltInfo->addLayout(ltTotal);

  QFrame* frInfo = new QFrame;
  frInfo->setLayout(ltInfo);

  m_teObservation = new QPlainTextEdit;
  QVBoxLayout* ltObservation = new QVBoxLayout;
  ltObservation->addWidget(m_teObservation);
  QFrame* frObservation = new QFrame;
  frObservation->setLayout(ltObservation);

  m_wPayment = new PaymentWidget(this);

  m_tab->addTab(frInfo, QIcon(":/icons/res/details.png"), tr("Informações"));
  m_tab->addTab(m_wPayment, m_wPayment->getIcon(), tr("Pagamento"));
  m_tab->addTab(frObservation, QIcon(":/icons/res/pencil.png"), tr("Observações"));

  m_filter = new PurchaseFilter;

  m_edEntries = new JLineEdit(JLineEdit::Input::All);
  m_edEntries->setReadOnly(true);
  m_edSum = new JLineEdit(JLineEdit::Input::All);
  m_edSum->setReadOnly(true);
  QFormLayout* ltDbInfo = new QFormLayout;
  ltDbInfo->addRow(tr("Número de compras:"), m_edEntries);
  ltDbInfo->addRow(tr("Soma das compras:"), m_edSum);
  QFrame* frDbInfo = new QFrame;
  frDbInfo->setLayout(ltDbInfo);

  m_tabDb->addTab(m_filter, QIcon(":/icons/res/filter.png"), tr("Filtro"));
  m_tabDb->addTab(frDbInfo, QIcon(":/icons/res/statistics.png"), tr("Estatísticas"));

  setContentsMargins(9, 9, 9, 9);
  connect(m_btnAddRemove->m_btnAdd, SIGNAL(clicked(bool)), this, SLOT(addProduct()));
  connect(m_btnAddRemove->m_btnRemove, SIGNAL(clicked(bool)), this, SLOT(removeProduct()));
  connect(m_table, SIGNAL(changedSignal(bool)), m_btnAddRemove->m_btnRemove, SLOT(setEnabled(bool)));
  connect(m_btnAddCode, SIGNAL(clicked(bool)), this, SLOT(addProduct()));
  connect(m_table, SIGNAL(changedSignal(bool)), this, SLOT(updateControls()));
  connect(m_btnOpenLast, SIGNAL(clicked(bool)), this, SLOT(lastItemSelected()));
  connect(m_supplierPicker, SIGNAL(changedSignal()), this, SLOT(supplierChanged()));
  connect(m_edDisccount, SIGNAL(editingFinished()), this, SLOT(updateControls()));
  connect(m_edDisccount, SIGNAL(enterSignal()), m_table, SLOT(setFocus()));
  connect(m_filter, SIGNAL(filterChangedSignal(const QString&)), m_database, SLOT(setDynamicFilter(const QString&)));
  connect(m_tabDb, SIGNAL(currentChanged(int)), this, SLOT(updateStatistics()));
  connect(m_wPayment, SIGNAL(methodChangedSignal()), this, SLOT(updateControls()));
  connect(m_edTotal, SIGNAL(valueChanged(double)), m_wPayment, SLOT(setPurchaseTotal(double)));
  connect(m_dtPicker, SIGNAL(dateChangedSignal(const QDate&)), m_wPayment, SLOT(setPurchaseDate(const QDate&)));
  connect(this, SIGNAL(itemSelectedSignal()), SLOT(updateControls()));

  setFocusWidgetOnCreate(m_supplierPicker);
  create();
  updateControls();
  updateStatistics();
}

PurchaseView::~PurchaseView()
{

}

void PurchaseView::removeProduct()
{
  m_table->removeItem();
  if (!m_table->hasItems())
    m_supplierPicker->setFocus();
  updateControls();
}

void PurchaseView::getItem(JItemSQL& o) const
{
  Purchase& _o = dynamic_cast<Purchase&>(o);
  _o.clear(true);
  _o.m_id = m_id;
  _o.m_date = m_dtPicker->getDate();
  _o.m_supplier.m_id = m_supplierPicker->getId();
  _o.m_paymentMethod = m_wPayment->getPaymentMethod();
  _o.m_vPayment = m_wPayment->getPayments();
  _o.m_observation = m_teObservation->toPlainText();
  _o.m_disccount = m_edDisccount->getValue();
  for (int i = 0; i != m_table->rowCount(); ++i)
    _o.m_vElement.push_back(dynamic_cast<const PurchaseElement&>(m_table->getItem(i)));
}

void PurchaseView::setItem(const JItemSQL& o)
{
  m_cbStore->refresh();
  const Purchase& _o = dynamic_cast<const Purchase&>(o);
  m_table->removeAllItems();
  m_supplierPicker->clear();
  m_dtPicker->setDate(_o.m_date);
  m_snNumber->setValue(_o.m_number);
  for (int i = 0; i != _o.m_vElement.size(); ++i)
    m_table->addItem(_o.m_vElement.at(i));
  m_supplierPicker->setItem(_o.m_supplier);
  m_teObservation->setPlainText(_o.m_observation);
  m_wPayment->setPaymentMethod(_o.m_paymentMethod);
  m_wPayment->setPayments(_o.m_vPayment);
  m_edDisccount->setText(_o.m_disccount);
  updateControls();
}

void PurchaseView::supplierChanged()
{
  if (m_supplierPicker->getId().isValid())
  {
    if (m_table->hasItems())
    {
      m_table->setCurrentCell(0, 0);
      m_table->setFocus();
    }
    else if (!m_id.isValid())
      m_btnAddRemove->m_btnAdd->click();
  }
  updateControls();
}

void PurchaseView::updateControls()
{
  m_btnOpenLast->setEnabled(m_lastId.isValid());
  double total = m_table->computeTotal() + m_edDisccount->getValue();
  m_edTotal->setText(total);
  m_tab->setTabIcon(1, m_wPayment->getIcon());
  emit changedSignal();
}

void PurchaseView::updateStatistics()
{
  m_edEntries->setText(JItem::st_strInt(m_database->getNumberOfEntries()));
  m_edSum->setText(JItem::st_strMoney(m_database->getSum(5)));
}

void PurchaseView::addProduct()
{
  m_table->addItemAndLoadPrices(m_supplierPicker->getId(), sender() == m_btnAddCode);
}

bool PurchaseView::save(Id& id)
{
  id.clear();
  Purchase o;
  getItem(o);

  // TODO por enquanto corrigimos o pagamento
  o.adjustPayment();

  if (!o.isPaymentValid())
  {
    QMessageBox::critical(this,
                          tr("Pagamento inconsistente"),
                          tr("O valor do pagamento é diferente do valor da nota."),
                          QMessageBox::Ok);
    return false;
  }

  bool bSuccess = m_database->save(o);
  if (bSuccess)
  {
    id = o.m_id;
    m_lastId = o.m_id;
    create();
  }
  updateControls();
  return bSuccess;
}

void PurchaseView::lastItemSelected()
{
  if (m_lastId.isValid())
    m_database->selectItem(m_lastId);
}

void PurchaseView::selectItem(const JItemSQL& o)
{
  if (!m_lastId.isValid() || o.m_id.isValid())
    m_lastId = o.m_id;
  JItemView::selectItem(o);
}

void PurchaseView::itemsRemoved(const QVector<Id>& ids)
{
  JItemView::itemsRemoved(ids);
  if (ids.contains(m_lastId))
    m_lastId.clear();
}

void PurchaseView::setDate(const QDate& dt)
{
  m_dtPicker->setDate(dt);
}

QDate PurchaseView::getDate() const
{
  return m_dtPicker->getDate();
}
