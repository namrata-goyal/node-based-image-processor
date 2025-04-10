#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "NodeGraph.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QListWidget>
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QMenu>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void addNode();
    void removeSelectedNode();
    void openImage();
    void saveImage();
    void showNodeProperties();
    void updatePropertiesPanel();
    
private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createDockWidgets();
    void setupNodeFactory();
    
    NodeGraph* m_graph;
    QGraphicsView* m_view;
    
    QDockWidget* m_nodeListDock;
    QListWidget* m_nodeList;
    
    QDockWidget* m_propertiesDock;
    QWidget* m_propertiesPanel;
    
    QToolBar* m_toolBar;
    
    QAction* m_addNodeAction;
    QAction* m_removeNodeAction;
    QAction* m_openImageAction;
    QAction* m_saveImageAction;
    
    Node* m_selectedNode = nullptr;
};

#endif // MAINWINDOW_H