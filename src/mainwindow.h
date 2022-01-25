#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "document.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	std::unique_ptr<Document> document;

	float GetMipLevel() const;
	float GetZScroll() const { return 0; }

	void SetStatusBarMessage(const char * m = nullptr);
	void SetStatusBarMessage(glm::ivec2 coords);

	glm::vec2 GetScroll();
	void      SetScroll(glm::vec2 scroll);

	float SetZoom(float zoom);
	float GetZoom() const { return m_zoom; }

	GLViewWidget * GetGl() const;

private:
	bool fileOpen();

friend class GLViewWidget;
	Ui::MainWindow *ui;

	float m_zoom{1.f};
	bool m_haveMessage{false};
};
#endif // MAINWINDOW_H
