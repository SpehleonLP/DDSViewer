#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <glm/glm.hpp>

static QDir g_defaultDir = QDir::home();

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->viewWidget->w = this;

	connect(ui->actionAbout_QT,    &QAction::triggered, &QApplication::aboutQt);
	connect(ui->actionExit,        &QAction::triggered, &QApplication::closeAllWindows);
	connect(ui->actionOpen,        &QAction::triggered, [this]() { fileOpen(); } );

	connect(ui->zoomActual,      &QAction::triggered, [this]()
	{
		SetZoom(.5f);
	});
    connect(ui->zoomIn,          &QAction::triggered, [this]()
	{
		SetZoom(m_zoom * 9.f/8.f);
	});
    connect(ui->zoomOut,         &QAction::triggered, [this]()
	{
		SetZoom(m_zoom * 7.f/8);
	});

	connect(ui->MipMap,      &QSpinBox::valueChanged, [this]()
	{
		ui->viewWidget->need_repaint();
	});
}

MainWindow::~MainWindow()
{
	delete ui;
}

bool MainWindow::fileOpen()
{
	ui->viewWidget->makeCurrent();

	QFileDialog dialog(this, tr("Load DDS File"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setNameFilter("DirectDrawSurface (*.dds)");
	dialog.setDirectory(g_defaultDir);

	bool accepted;
	while ((accepted = (dialog.exec() == QDialog::Accepted)))
	{
		try
		{
			auto Path = QFileInfo(dialog.selectedFiles().first());
			document = Document::Factory(Path.filePath().toStdString(), this);

			if(document)
			{
				ui->fourCC->setText(QString::fromStdString(document->GetFourCC()));
				ui->dxgiFormat->setText(document->GetDxgiFormat());
				ui->Dimension->setText(document->GetDimension());
				ui->Width->setText(QString::number(document->GetDimensions().x));
				ui->Height->setText(QString::number(document->GetDimensions().y));
				ui->Pitch->setText(QString::number(document->GetPitch()));
				ui->RgbBitCount->setText(QString::number(document->GetBitDepth()));
			//	ui->MipMap->setMaximum(document->GetMipMaps()+1);
			}
			setWindowTitle(Path.fileName());

			break;
		}
		catch(std::exception & e)
		{
			if(QMessageBox::Ok == QMessageBox::warning(this, "Failed to open file.", e.what() + tr("\n try again?")))
				return false;
		}
	}

	if(!accepted)
		return false;

	g_defaultDir = dialog.directory();


	ui->viewWidget->need_repaint();

	return true;
}

void MainWindow::SetStatusBarMessage(const char * m)
{
	statusBar()->showMessage(m);
	m_haveMessage = (m != nullptr);
}

void MainWindow::SetStatusBarMessage(glm::ivec2 coords)
{
	if(!m_haveMessage)
	{
		statusBar()->showMessage(QString("x: %1 y: %2").arg(coords.x).arg(coords.y));
	}
}

float MainWindow::GetMipLevel() const
{
	return ui->MipMap->value();
}



glm::vec2  MainWindow::GetScroll()
{
	return glm::vec2(
		(ui->horizontalScrollBar->value() - ui->horizontalScrollBar->minimum())
			/ (double) (ui->horizontalScrollBar->maximum() - ui->horizontalScrollBar->minimum()),
		 1 - (ui->verticalScrollBar->value() - ui->verticalScrollBar->minimum())
			/ (double) (ui->verticalScrollBar->maximum() - ui->verticalScrollBar->minimum()));
}

void  MainWindow::SetScroll(glm::vec2 scroll)
{
	scroll = glm::max(glm::vec2(0), glm::min(scroll, glm::vec2(1)));
	int scroll_x = glm::mix(ui->horizontalScrollBar->minimum(), ui->horizontalScrollBar->maximum(), scroll.x);
	int scroll_y = glm::mix(ui->verticalScrollBar->minimum(), ui->verticalScrollBar->maximum(), 1 - scroll.y);

	ui->horizontalScrollBar->setValue(scroll_x);
	ui->verticalScrollBar->setValue(scroll_y);
}

float MainWindow::SetZoom(float zoom)
{
const float g_MinZoom = .125f;
const float g_MaxZoom = 2.f;

	float p_zoom = m_zoom;

	m_zoom = std::max(g_MinZoom, std::min(g_MaxZoom, zoom));

	ui->zoomIn->setEnabled(m_zoom < g_MaxZoom);
	ui->zoomOut->setEnabled(m_zoom > g_MinZoom);

	ui->horizontalScrollBar->setPageStep(m_zoom*64);
	ui->verticalScrollBar->setPageStep(m_zoom*64);

	ui->viewWidget->need_repaint();

	return p_zoom / m_zoom;
}

GLViewWidget * MainWindow::GetGl() const
{
	return ui->viewWidget;
}
