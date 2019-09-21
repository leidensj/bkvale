#include "employeeview.h"
#include "widgets/jlineedit.h"
#include "widgets/jdatabasepicker.h"
#include "widgets/jspinbox.h"
#include "widgets/jtimeedit.h"
#include "widgets/jaddremovebuttons.h"
#include "tables/employeehourstable.h"
#include "items/jitemex.h"
#include <QFormLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include "timeinterval.h"
#include <QHeaderView>
#include <QRadioButton>
#include <QTreeWidget>
#include "formwidget.h"

EmployeeView::EmployeeView(QWidget* parent)
  : JItemView(EMPLOYEE_SQL_TABLE_NAME, parent)
  , m_formInfo(nullptr)
  , m_formDetails(nullptr)
  , m_formPhone(nullptr)
  , m_formAddress(nullptr)
  , m_edPincode(nullptr)
  , m_trPermissions(nullptr)
  , m_triPurchaseEdit(nullptr)
  , m_triPurchaseRemove(nullptr)
  , m_tbHours(nullptr)
  , m_btnAddRemove(nullptr)
{
  m_formInfo = new FormInfoWidget;
  m_formDetails = new FormDetailsWidget;
  m_formPhone = new FormPhoneWidget;
  m_formAddress= new FormAddressWidget;
  m_edPincode = new JLineEdit(Text::Input::Alphanumeric);
  m_btnAddRemove = new JAddRemoveButtons;
  m_tbHours = new EmployeeHoursTable(m_btnAddRemove);

  m_trPermissions = new QTreeWidget;
  m_trPermissions->setColumnCount(1);
  m_trPermissions->header()->setVisible(false);
  auto root = new QTreeWidgetItem;
  root->setText(0, JItemEx::text(PURCHASE_SQL_TABLE_NAME));
  root->setIcon(0, QIcon(JItemEx::icon(PURCHASE_SQL_TABLE_NAME)));
  m_trPermissions->insertTopLevelItem(0, root);
  m_triPurchaseEdit = new QTreeWidgetItem;
  m_triPurchaseRemove = new QTreeWidgetItem;
  m_triPurchaseEdit->setText(0, tr("Criar/editar"));
  m_triPurchaseEdit->setIcon(0, QIcon(":/icons/res/file.png"));
  m_triPurchaseEdit->setCheckState(0, Qt::Unchecked);
  m_triPurchaseRemove->setText(0, tr("Remover"));
  m_triPurchaseRemove->setIcon(0, QIcon(":/icons/res/remove.png"));
  m_triPurchaseRemove->setCheckState(0, Qt::Unchecked);
  root->addChild(m_triPurchaseEdit);
  root->addChild(m_triPurchaseRemove);
  root->setExpanded(true);

  m_formInfo->addWidget(tr("Código PIN:"), m_edPincode);

  QVBoxLayout* ltPermissions = new QVBoxLayout;
  ltPermissions->addWidget(m_trPermissions);

  QFrame* frPermissions = new QFrame;
  frPermissions->setLayout(ltPermissions);

  QVBoxLayout* ltHours = new QVBoxLayout;
  ltHours->addWidget(m_btnAddRemove);
  ltHours->addWidget(m_tbHours);

  QFrame* frHours = new QFrame;
  frHours->setLayout(ltHours);

  m_tab->addTab(m_formInfo, QIcon(":/icons/res/resume.png"), tr("Informações"));
  m_tab->addTab(m_formDetails, QIcon(":/icons/res/details.png"), tr("Detalhes"));
  m_tab->addTab(frPermissions, QIcon(":/icons/res/usershield.png"), tr("Permissões"));
  m_tab->addTab(frHours, QIcon(":/icons/res/clock.png"), tr("Horário"));
  m_tab->addTab(m_formPhone, QIcon(":/icons/res/phone.png"), tr("Telefone"));
  m_tab->addTab(m_formAddress, QIcon(":/icons/res/address.png"), tr("Endereço"));

  connect(m_formInfo, SIGNAL(userTypeChangedSignal(bool)), m_formDetails, SLOT(switchUserType(bool)));

  m_formInfo->m_lblCreationDate->hide();
  m_formInfo->m_dtCreationDate->hide();
  m_formInfo->m_lblType->hide();
  m_formInfo->m_type->hide();
  m_formInfo->setCompany(false);

  setFocusWidgetOnCreate(m_formInfo->m_edName);
  create();
}

void EmployeeView::getItem(JItemSQL& o) const
{
  Employee& _o = dynamic_cast<Employee&>(o);
  _o.clear(true);
  _o.m_id = m_id;
  _o.m_form.m_id = m_formId;
  m_formInfo->fillForm(_o.m_form);
  m_formDetails->fillForm(_o.m_form);
  m_formPhone->fillForm(_o.m_form);
  m_formAddress->fillForm(_o.m_form);
  _o.m_pincode = m_edPincode->text();
  _o.m_bPurchaseEdit = m_triPurchaseEdit->checkState(0) == Qt::Checked;
  _o.m_bPurchaseRemove = m_triPurchaseRemove->checkState(0) == Qt::Checked;
  m_tbHours->getHours(_o.m_hours);
}

void EmployeeView::setItem(const JItemSQL& o)
{
  const Employee& _o = dynamic_cast<const Employee&>(o);
  m_formId = _o.m_form.m_id;
  m_formInfo->setForm(_o.m_form);
  m_formDetails->setForm(_o.m_form);
  m_formPhone->setForm(_o.m_form);
  m_formAddress->setForm(_o.m_form);
  m_edPincode->setText(_o.m_pincode);
  m_triPurchaseEdit->setCheckState(0, _o.m_bPurchaseEdit ? Qt::Checked : Qt::Unchecked);
  m_triPurchaseRemove->setCheckState(0, _o.m_bPurchaseRemove ? Qt::Checked : Qt::Unchecked);
  m_tbHours->setHours(_o.m_hours);
}
