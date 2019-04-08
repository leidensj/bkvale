#include "userview.h"
#include "jitemhelper.h"
#include <QCheckBox>
#include "jlineedit.h"
#include <QLabel>
#include <QLayout>
#include <QRegExpValidator>
#include <QGroupBox>
#include <QMessageBox>

UserView::UserView(Id currentLoggedId, QWidget* parent)
  : JItemView(USER_SQL_TABLE_NAME, parent)
  , m_currentLoggedId(currentLoggedId)
  , m_bHasLoggedUserChanged(false)
  , m_user(nullptr)
  , m_lblPasswordMsg(nullptr)
  , m_password(nullptr)
  , m_viewPassword(nullptr)
  , m_accessNote(nullptr)
  , m_accessReminder(nullptr)
  , m_accessCalculator(nullptr)
  , m_accessShoppingList(nullptr)
  , m_accessShop(nullptr)
  , m_accessUser(nullptr)
  , m_accessProduct(nullptr)
  , m_accessForm(nullptr)
  , m_accessEmployee(nullptr)
  , m_accessSupplier(nullptr)
  , m_accessCategory(nullptr)
  , m_accessImage(nullptr)
  , m_accessReservation(nullptr)
  , m_accessSettings(nullptr)
{
  QLabel* lblUser = new QLabel;
  lblUser->setPixmap(QIcon(":/icons/res/user.png").pixmap(QSize(24, 24)));
  lblUser->setMinimumSize(24, 24);
  lblUser->setMaximumSize(24, 24);
  lblUser->setScaledContents(true);
  QLabel* lblPassword = new QLabel;
  lblPassword->setPixmap(QIcon(":/icons/res/password.png").pixmap(QSize(24, 24)));
  lblPassword->setMinimumSize(24, 24);
  lblPassword->setMaximumSize(24, 24);
  lblPassword->setScaledContents(true);

  m_user = new JLineEdit(JLineEdit::Input::Alphanumeric, JLineEdit::st_defaultFlags1);
  m_user->setPlaceholderText(tr("Usuário"));
  m_user->setMaxLength(USER_MAX_USERNAME_LENGTH);

  m_lblPasswordMsg = new QLabel;
  m_lblPasswordMsg->setText(tr("Redefinir a senha:"));

  m_password = new JLineEdit(JLineEdit::Input::All, JLineEdit::st_defaultFlags2);
  m_password->setPlaceholderText(tr("Senha"));
  m_user->setMaxLength(USER_MAX_PASSWORD_LENGTH);
  m_password->setEchoMode(QLineEdit::EchoMode::Password);

  m_viewPassword = new QPushButton;
  m_viewPassword->setFlat(true);
  m_viewPassword->setText("");
  m_viewPassword->setIconSize(QSize(16, 16));
  m_viewPassword->setIcon(QIcon(":/icons/res/view.png"));
  m_viewPassword->setCheckable(true);

  m_accessNote = new QCheckBox;
  m_accessNote->setIcon(QIcon(":/icons/res/note.png"));
  m_accessNote->setText(tr("Vales"));
  m_accessReminder = new QCheckBox;
  m_accessReminder->setIcon(QIcon(":/icons/res/postit.png"));
  m_accessReminder->setText(tr("Lembretes"));
  m_accessCalculator = new QCheckBox;
  m_accessCalculator->setIcon(QIcon(":/icons/res/calculator.png"));
  m_accessCalculator->setText(tr("Calculadora"));
  m_accessShop = new QCheckBox;
  m_accessShop->setIcon(QIcon(":/icons/res/shop.png"));
  m_accessShop->setText(tr("Compras"));
  m_accessShoppingList = new QCheckBox;
  m_accessShoppingList->setIcon(QIcon(":/icons/res/shopmgt.png"));
  m_accessShoppingList->setText(tr("Listas de Compras"));

  m_accessUser = new QCheckBox;
  m_accessUser->setIcon(QIcon(JItemHelper::icon(USER_SQL_TABLE_NAME)));
  m_accessUser->setText(JItemHelper::text(USER_SQL_TABLE_NAME));

  m_accessProduct = new QCheckBox;
  m_accessProduct->setIcon(QIcon(JItemHelper::icon(PRODUCT_SQL_TABLE_NAME)));
  m_accessProduct->setText(JItemHelper::text(PRODUCT_SQL_TABLE_NAME));

  m_accessForm = new QCheckBox;
  m_accessForm->setIcon(QIcon(JItemHelper::icon(FORM_SQL_TABLE_NAME)));
  m_accessForm->setText(JItemHelper::text(FORM_SQL_TABLE_NAME));

  m_accessEmployee = new QCheckBox;
  m_accessEmployee->setIcon(QIcon(JItemHelper::icon(EMPLOYEE_SQL_TABLE_NAME)));
  m_accessEmployee->setText(JItemHelper::text(EMPLOYEE_SQL_TABLE_NAME));

  m_accessSupplier = new QCheckBox;
  m_accessSupplier->setIcon(QIcon(JItemHelper::icon(SUPPLIER_SQL_TABLE_NAME)));
  m_accessSupplier->setText(JItemHelper::text(SUPPLIER_SQL_TABLE_NAME));

  m_accessCategory = new QCheckBox;
  m_accessCategory->setIcon(QIcon(JItemHelper::icon(CATEGORY_SQL_TABLE_NAME)));
  m_accessCategory->setText(JItemHelper::text(CATEGORY_SQL_TABLE_NAME));

  m_accessImage = new QCheckBox;
  m_accessImage->setIcon(QIcon(JItemHelper::icon(IMAGE_SQL_TABLE_NAME)));
  m_accessImage->setText(JItemHelper::text(IMAGE_SQL_TABLE_NAME));

  m_accessReservation = new QCheckBox;
  m_accessReservation->setIcon(QIcon(JItemHelper::icon(RESERVATION_SQL_TABLE_NAME)));
  m_accessReservation->setText(JItemHelper::text(RESERVATION_SQL_TABLE_NAME));

  m_accessSettings = new QCheckBox;
  m_accessSettings->setIcon(QIcon(":/icons/res/settings.png"));
  m_accessSettings->setText(tr("Configurações"));

  QHBoxLayout* userlayout = new QHBoxLayout;
  userlayout->setContentsMargins(0, 0, 0, 0);
  userlayout->addWidget(lblUser);
  userlayout->addWidget(m_user);

  QHBoxLayout* passwordlayout = new QHBoxLayout;
  passwordlayout->setContentsMargins(0, 0, 0, 0);
  passwordlayout->addWidget(lblPassword);
  passwordlayout->addWidget(m_password);
  passwordlayout->addWidget(m_viewPassword);

  QVBoxLayout* tabPermissionslayout = new QVBoxLayout;
  tabPermissionslayout->addWidget(m_accessNote);
  tabPermissionslayout->addWidget(m_accessReminder);
  tabPermissionslayout->addWidget(m_accessCalculator);
  tabPermissionslayout->addWidget(m_accessShop);
  tabPermissionslayout->addWidget(m_accessReservation);
  tabPermissionslayout->addWidget(m_accessUser);
  tabPermissionslayout->addWidget(m_accessProduct);
  tabPermissionslayout->addWidget(m_accessForm);
  tabPermissionslayout->addWidget(m_accessEmployee);
  tabPermissionslayout->addWidget(m_accessSupplier);
  tabPermissionslayout->addWidget(m_accessCategory);
  tabPermissionslayout->addWidget(m_accessImage);
  tabPermissionslayout->addWidget(m_accessShoppingList);
  tabPermissionslayout->addWidget(m_accessSettings);

  QVBoxLayout* tabUserlayout = new QVBoxLayout;
  tabUserlayout->setAlignment(Qt::AlignTop);
  tabUserlayout->addLayout(userlayout);
  tabUserlayout->addWidget(m_lblPasswordMsg);
  tabUserlayout->addLayout(passwordlayout);

  QFrame* tabUserFrame = new QFrame;
  tabUserFrame->setLayout(tabUserlayout);

  QFrame* tabPermissionsFrame = new QFrame;
  tabPermissionsFrame->setLayout(tabPermissionslayout);

  m_tab->addTab(tabUserFrame,
                QIcon(":/icons/res/user.png"),
                tr("Usuário"));

  m_tab->addTab(tabPermissionsFrame,
                QIcon(":/icons/res/usershield.png"),
                tr("Permissões"));

  QObject::connect(m_viewPassword,
                   SIGNAL(toggled(bool)),
                   this,
                   SLOT(viewPassword(bool)));

  create();
}

const JItemSQL& UserView::getItem() const
{
  m_ref.clear();
  m_ref.m_id = m_currentId;
  m_ref.m_strUser = m_user->text();
  m_ref.m_password = m_password->text();
  m_ref.m_bAccessNote = m_accessNote->isChecked();
  m_ref.m_bAccessReminder = m_accessReminder->isChecked();
  m_ref.m_bAccessCalculator = m_accessCalculator->isChecked();
  m_ref.m_bAccessShop = m_accessShop->isChecked();
  m_ref.m_bAccessUser = m_accessUser->isChecked();
  m_ref.m_bAccessProduct = m_accessProduct->isChecked();
  m_ref.m_bAccessForm = m_accessForm->isChecked();
  m_ref.m_bAccessEmployee = m_accessEmployee->isChecked();
  m_ref.m_bAccessSupplier = m_accessSupplier->isChecked();
  m_ref.m_bAccessCategory = m_accessCategory->isChecked();
  m_ref.m_bAccessImage = m_accessImage->isChecked();
  m_ref.m_bAccessSettings = m_accessSettings->isChecked();
  m_ref.m_bAccessReservation = m_accessReservation->isChecked();
  m_ref.m_bAccessShoppingList = m_accessShoppingList->isChecked();
  return m_ref;
}

void UserView::setItem(const JItemSQL& o)
{
  auto _o = dynamic_cast<const User&>(o);
  if (_o.m_id.isValid())
    m_lblPasswordMsg->show();
  m_currentId = _o.m_id;
  m_user->setText(_o.m_strUser);
  m_password->setText("");
  m_accessNote->setChecked(_o.m_bAccessNote);
  m_accessReminder->setChecked(_o.m_bAccessReminder);
  m_accessCalculator->setChecked(_o.m_bAccessCalculator);
  m_accessShop->setChecked(_o.m_bAccessShop);
  m_accessUser->setChecked(_o.m_bAccessUser);
  m_accessProduct->setChecked(_o.m_bAccessProduct);
  m_accessForm->setChecked(_o.m_bAccessForm);
  m_accessEmployee->setChecked(_o.m_bAccessEmployee);
  m_accessSupplier->setChecked(_o.m_bAccessSupplier);
  m_accessCategory->setChecked(_o.m_bAccessCategory);
  m_accessImage->setChecked(_o.m_bAccessImage);
  m_accessSettings->setChecked(_o.m_bAccessSettings);
  m_accessReservation->setChecked(_o.m_bAccessReservation);
  m_accessShoppingList->setChecked(_o.m_bAccessShoppingList);
}

void UserView::create()
{
  selectItem(User());
  m_lblPasswordMsg->hide();
  m_user->setFocus();
}

QString UserView::getPassword() const
{
  return m_password->text();
}

void UserView::viewPassword(bool b)
{
  m_password->setEchoMode( b ? QLineEdit::EchoMode::Normal
                             : QLineEdit::EchoMode::Password);
}

void UserView::itemsRemoved(const QVector<Id>& ids)
{
  if (!m_bHasLoggedUserChanged)
    m_bHasLoggedUserChanged = ids.contains(m_bHasLoggedUserChanged);
  JItemView::itemsRemoved(ids);
}

void UserView::save()
{
  if (!m_bHasLoggedUserChanged)
    m_bHasLoggedUserChanged = m_currentId == m_currentLoggedId;
  JItemView::save();
}

bool UserView::hasLoggedUserChanged() const
{
  return m_bHasLoggedUserChanged;
}