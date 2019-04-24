#include "storeview.h"
#include "jlineedit.h"
#include "jplaintextedit.h"
#include "formwidget.h"
#include <QLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QRadioButton>
#include <QLabel>
#include <QDateEdit>

StoreView::StoreView(QWidget* parent)
  : JItemView(STORE_SQL_TABLE_NAME, parent)
  , m_formInfo(nullptr)
  , m_formDetails(nullptr)
  , m_formPhone(nullptr)
  , m_formAddress(nullptr)
{
  m_formInfo = new FormInfoWidget;
  m_formDetails = new FormDetailsWidget;
  m_formPhone = new FormPhoneWidget;
  m_formAddress= new FormAddressWidget;
  m_list = new QListWidget;

  m_tab->addTab(m_formInfo, QIcon(":/icons/res/resume.png"), tr("Informações"));
  m_tab->addTab(m_formDetails, QIcon(":/icons/res/details.png"), tr("Detalhes"));
  m_tab->addTab(m_formPhone, QIcon(":/icons/res/phone.png"), tr("Telefone"));
  m_tab->addTab(m_formAddress, QIcon(":/icons/res/address.png"), tr("Endereço"));
  m_tab->addTab(m_list, QIcon(":/icons/res/employee.png"), tr("Funcionários"));

  connect(m_formInfo, SIGNAL(userTypeChangedSignal(bool)), m_formDetails, SLOT(switchUserType(bool)));

  m_formInfo->m_lblCreationDate->hide();
  m_formInfo->m_dtCreationDate->hide();
  m_formInfo->m_lblType->hide();
  m_formInfo->m_type->hide();
  m_formInfo->setCompany(true);
  updateControls();
}

void StoreView::create()
{
  selectItem(Store());
  updateControls();
}

void StoreView::updateControls()
{
  //TODO
}

const JItemSQL& StoreView::getItem() const
{
  m_ref.clear();
  m_ref.m_id = m_currentId;
  m_formInfo->fillForm(m_ref.m_form);
  m_formDetails->fillForm(m_ref.m_form);
  m_formPhone->fillForm(m_ref.m_form);
  m_formAddress->fillForm(m_ref.m_form);
  return m_ref;
}

void StoreView::setItem(const JItemSQL& o)
{
  const Store& ref = static_cast<const Store&>(o);
  m_currentId = o.m_id;
  m_formInfo->setForm(ref.m_form);
  m_formDetails->setForm(ref.m_form);
  m_formPhone->setForm(ref.m_form);
  m_formAddress->setForm(ref.m_form);
  m_list->clear();
  m_list->addItems(ref.SQL_select_employees());
}
