#include "imageview.h"
#include "jlineedit.h"
#include "jimageview.h"
#include <QPushButton>
#include <QLayout>

ImageView::ImageView(QWidget* parent)
  : QFrame(parent)
  , m_btnCreate(nullptr)
  , m_btnSave(nullptr)
  , m_edImageName(nullptr)
  , m_imageView(nullptr)
{
  m_btnCreate = new QPushButton();
  m_btnCreate->setFlat(true);
  m_btnCreate->setText("");
  m_btnCreate->setIconSize(QSize(24, 24));
  m_btnCreate->setIcon(QIcon(":/icons/res/file.png"));
  m_btnCreate->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));

  m_btnSave = new QPushButton();
  m_btnSave->setFlat(true);
  m_btnSave->setText("");
  m_btnSave->setIconSize(QSize(24, 24));
  m_btnSave->setIcon(QIcon(":/icons/res/save.png"));
  m_btnSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));

  m_edImageName = new JLineEdit(JValidatorType::AlphanumericAndSpaces, true, true);
  m_edImageName->setPlaceholderText(tr("Nome"));

  m_imageView = new JImageView(true);

  QHBoxLayout* hlayout0 = new QHBoxLayout();
  hlayout0->setContentsMargins(0, 0, 0, 0);
  hlayout0->setAlignment(Qt::AlignLeft);
  hlayout0->addWidget(m_btnCreate);
  hlayout0->addWidget(m_btnSave);

  QVBoxLayout* vlayout0 = new QVBoxLayout();
  vlayout0->addLayout(hlayout0);
  vlayout0->addWidget(m_edImageName);
  vlayout0->addWidget(m_imageView);
  setLayout(vlayout0);

  QObject::connect(m_btnCreate,
                   SIGNAL(clicked(bool)),
                   this,
                   SLOT(create()));

  QObject::connect(m_btnSave,
                   SIGNAL(clicked(bool)),
                   this,
                   SLOT(emitSaveSignal()));

  QObject::connect(m_edImageName,
                   SIGNAL(textChanged(const QString&)),
                   this,
                   SLOT(updateControls()));

  QObject::connect(m_imageView,
                   SIGNAL(changedSignal()),
                   this,
                   SLOT(updateControls()));

  updateControls();
}

 void ImageView::setImage(const Image& image)
 {
   m_edImageName->clear();
   m_imageView->clearImage();
   m_currentImg = image;
   m_edImageName->setText(m_currentImg.m_name);
   m_imageView->setImage(m_currentImg.m_image);
   updateControls();
 }

 Image ImageView::getImage() const
 {
   Image img;
   img.m_id = m_currentImg.m_id;
   img.m_name = m_edImageName->text();
   img.m_image = m_imageView->getImage();
   return img;
 }

 void ImageView::create()
 {
   m_currentImg = Image();
   m_edImageName->clear();
   m_imageView->clearImage();
   updateControls();
 }

 void ImageView::emitSaveSignal()
 {
  emit saveSignal();
 }

 void ImageView::updateControls()
 {
   Image img = getImage();
   bool bEnable = img.isValid();
   QString saveIcon(":/icons/res/save.png");
   if (m_currentImg.isValidId())
   {
     saveIcon = ":/icons/res/saveas.png";
     bEnable = bEnable && m_currentImg != img;
   }

   m_btnSave->setEnabled(bEnable);
   m_btnSave->setIcon(QIcon(saveIcon));
 }
