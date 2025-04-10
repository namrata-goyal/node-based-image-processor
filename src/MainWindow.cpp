void MainWindow::setupNodeFactory() {
    // Node types are already registered via the REGISTER_NODE macros
    // This function is kept for potential future extensions
}

void MainWindow::addNode() {
    if (m_nodeList->currentItem()) {
        Node* node = NodeFactory::instance().createNode(m_nodeList->currentItem()->text().toStdString());
        if (node) {
            node->setPos(m_view->mapToScene(m_view->viewport()->rect().center()));
            m_graph->addNode(node);
        }
    }
}

void MainWindow::removeSelectedNode() {
    QList<QGraphicsItem*> items = m_graph->selectedItems();
    for (auto item : items) {
        if (Node* node = dynamic_cast<Node*>(item)) {
            m_graph->removeNode(node);
        }
    }
}

void MainWindow::openImage() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image", "", 
        "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty()) {
        // Find or create an ImageInputNode
        ImageInputNode* inputNode = nullptr;
        for (auto node : m_graph->getNodes()) {
            if (ImageInputNode* imgNode = dynamic_cast<ImageInputNode*>(node)) {
                inputNode = imgNode;
                break;
            }
        }
        
        if (!inputNode) {
            inputNode = new ImageInputNode();
            inputNode->setPos(m_view->mapToScene(m_view->viewport()->rect().center()));
            m_graph->addNode(inputNode);
        }
        
        inputNode->setImagePath(filePath.toStdString());
        m_graph->processGraph();
    }
}

void MainWindow::saveImage() {
    // Find an output node
    ImageOutputNode* outputNode = nullptr;
    for (auto node : m_graph->getNodes()) {
        if (ImageOutputNode* outNode = dynamic_cast<ImageOutputNode*>(node)) {
            outputNode = outNode;
            break;
        }
    }
    
    if (!outputNode) {
        QMessageBox::warning(this, "No Output Node", "Please add an Image Output node to the graph");
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(this, "Save Image", "", 
        "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
    if (!filePath.isEmpty()) {
        outputNode->setOutputPath(filePath.toStdString());
        m_graph->processGraph();
        QMessageBox::information(this, "Success", "Image saved successfully");
    }
}

void MainWindow::showNodeProperties() {
    updatePropertiesPanel();
}

void MainWindow::updatePropertiesPanel() {
    // Clear the properties panel
    QLayoutItem* child;
    while ((child = m_propertiesPanel->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    
    // Find the first selected node
    m_selectedNode = nullptr;
    QList<QGraphicsItem*> items = m_graph->selectedItems();
    for (auto item : items) {
        if (Node* node = dynamic_cast<Node*>(item)) {
            m_selectedNode = node;
            break;
        }
    }
    
    if (!m_selectedNode) {
        QLabel* label = new QLabel("No node selected", m_propertiesPanel);
        m_propertiesPanel->layout()->addWidget(label);
        return;
    }
    
    // Create a form layout for the properties
    QFormLayout* formLayout = new QFormLayout();
    m_propertiesPanel->setLayout(formLayout);
    
    // Add node name label
    QLabel* nameLabel = new QLabel(QString::fromStdString(m_selectedNode->name()), m_propertiesPanel);
    QFont font = nameLabel->font();
    font.setBold(true);
    nameLabel->setFont(font);
    formLayout->addRow(nameLabel);
    
    // Add properties based on node type
    if (ImageInputNode* node = dynamic_cast<ImageInputNode*>(m_selectedNode)) {
        QPushButton* loadButton = new QPushButton("Load Image", m_propertiesPanel);
        connect(loadButton, &QPushButton::clicked, this, &MainWindow::openImage);
        formLayout->addRow(loadButton);
        
        // Display image info if loaded
        cv::Mat image = node->getImage();
        if (!image.empty()) {
            formLayout->addRow(new QLabel("Dimensions:", m_propertiesPanel));
            formLayout->addRow(new QLabel(QString("%1 x %2").arg(image.cols).arg(image.rows), m_propertiesPanel));
            formLayout->addRow(new QLabel("Channels:", m_propertiesPanel));
            formLayout->addRow(new QLabel(QString::number(image.channels())), m_propertiesPanel));
        }
    }
    else if (ImageOutputNode* node = dynamic_cast<ImageOutputNode*>(m_selectedNode)) {
        QPushButton* saveButton = new QPushButton("Save Image", m_propertiesPanel);
        connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);
        formLayout->addRow(saveButton);
    }
    else if (BrightnessContrastNode* node = dynamic_cast<BrightnessContrastNode*>(m_selectedNode)) {
        QSlider* brightnessSlider = new QSlider(Qt::Horizontal, m_propertiesPanel);
        brightnessSlider->setRange(-100, 100);
        brightnessSlider->setValue(0);
        connect(brightnessSlider, &QSlider::valueChanged, node, &BrightnessContrastNode::setBrightness);
        formLayout->addRow("Brightness:", brightnessSlider);
        
        QSlider* contrastSlider = new QSlider(Qt::Horizontal, m_propertiesPanel);
        contrastSlider->setRange(0, 300); // 0.0 to 3.0 in steps of 0.01
        contrastSlider->setValue(100); // 1.0
        connect(contrastSlider, &QSlider::valueChanged, [node](int value) {
            node->setContrast(value / 100.0f);
        });
        formLayout->addRow("Contrast:", contrastSlider);
    }
    else if (BlurNode* node = dynamic_cast<BlurNode*>(m_selectedNode)) {
        QSlider* radiusSlider = new QSlider(Qt::Horizontal, m_propertiesPanel);
        radiusSlider->setRange(1, 20);
        radiusSlider->setValue(5);
        connect(radiusSlider, &QSlider::valueChanged, node, &BlurNode::setRadius);
        formLayout->addRow("Blur Radius:", radiusSlider);
    }
    else if (ThresholdNode* node = dynamic_cast<ThresholdNode*>(m_selectedNode)) {
        QSlider* thresholdSlider = new QSlider(Qt::Horizontal, m_propertiesPanel);
        thresholdSlider->setRange(0, 255);
        thresholdSlider->setValue(127);
        connect(thresholdSlider, &QSlider::valueChanged, node, &ThresholdNode::setThreshold);
        formLayout->addRow("Threshold:", thresholdSlider);
    }
    else if (EdgeDetectionNode* node = dynamic_cast<EdgeDetectionNode*>(m_selectedNode)) {
        QComboBox* methodCombo = new QComboBox(m_propertiesPanel);
        methodCombo->addItem("Sobel");
        methodCombo->addItem("Canny");
        connect(methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                node, &EdgeDetectionNode::setMethod);
        formLayout->addRow("Method:", methodCombo);
    }
    else if (BlendNode* node = dynamic_cast<BlendNode*>(m_selectedNode)) {
        QComboBox* modeCombo = new QComboBox(m_propertiesPanel);
        modeCombo->addItem("Normal");
        modeCombo->addItem("Multiply");
        modeCombo->addItem("Screen");
        modeCombo->addItem("Overlay");
        modeCombo->addItem("Difference");
        connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                node, &BlendNode::setBlendMode);
        formLayout->addRow("Blend Mode:", modeCombo);
        
        QSlider* opacitySlider = new QSlider(Qt::Horizontal, m_propertiesPanel);
        opacitySlider->setRange(0, 100);
        opacitySlider->setValue(50);
        connect(opacitySlider, &QSlider::valueChanged, [node](int value) {
            node->setOpacity(value / 100.0f);
        });
        formLayout->addRow("Opacity:", opacitySlider);
    }
    
    // Add a spacer to push everything up
    formLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}