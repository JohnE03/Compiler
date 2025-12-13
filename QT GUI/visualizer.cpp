//
// Created by kiro3 on 12/11/2025.
//
#include "visualizer.h"
#include <QtWidgets>

#include <map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Spacing values (use the defaults from header)
static int H_UNIT = H_UNIT_DEFAULT;
static int V_UNIT = V_UNIT_DEFAULT;
static int NODE_W  = NODE_W_DEFAULT;
static int NODE_H  = NODE_H_DEFAULT;


struct TreeNodeVisual {
    double x;      // pixel X position
    int y;      // pixel Y position
    string label;   // text drawn inside the node
    string shape;   // "rect" or "ellipse"

    TreeNodeVisual(double X, int Y, const string& L, const string& S)
            : x(X), y(Y), label(L), shape(S) {}
};
// --- replace pair<int,int> edges with this Edge struct ---
struct Edge {
    int from;
    int to;
    bool isSibling;
    Edge(int f,int t,bool s) : from(f), to(t), isSibling(s) {}
};


// Trim whitespace from both ends (used for cleaner labels)
static string trim(const string& s) {
    size_t a = 0;
    while (a < s.size() && isspace((unsigned char)s[a])) a++;

    size_t b = s.size();
    while (b > a && isspace((unsigned char)s[b - 1])) b--;

    return s.substr(a, b - a);
}

// Lowercase copy (used to compare token types)
static string lower(const string& s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}
// -------------------- Positioning algorithm --------------------
// Leaves get unit positions 0,1,2,... ; parents are centered between their outer children.
// Pixel X = unit * H_UNIT, Pixel Y = depth * V_UNIT

// Post-order traversal: assign integer "unit X" index to each node
static void computeUnitX(Node* node, map<Node*, double>& unitX, double& leafCounter) {
    if (!node) return;

    // If no children → leaf node: give next unit index
    if (node->children.empty()) {
        unitX[node] = leafCounter++;

    }

    // Recurse into children first
    for (auto* ch : node->children) {
        if(node->token.type == ASSIGN || node->token.type == READ) {
            // Skip the first child (identifier) for unitX computation
            if (ch == node->children[0]) continue;
        }
        computeUnitX(ch, unitX, leafCounter);
    }

    // If children exist, center between first and last child
    if(node->token.type==ASSIGN)
    {
        unitX[node]= unitX[node->children[1]];
    }
    else if(node->token.type==READ)
    {
        unitX[node]= leafCounter++;
    }
    else if (node->children.empty()) {
        // already handled leaf case above
    } else {
        int leftUnit = unitX[node->children.front()];
        int rightUnit = unitX[node->children.back()];
        unitX[node] = (leftUnit + rightUnit) / 2.0;
    }

    if (node->sibling!= nullptr) {
        computeUnitX(node->sibling, unitX, leafCounter);
    }
}

// Convert the tree into drawable nodes (pixel coords) and edges (index pairs).
// nodeIndex maps SyntaxTreeNode* -> index in outNodes.
// Replace previous declaration/definition of collectNodesAndEdges with this version
static void collectNodesAndEdges(Node* node,
                                 const map<Node*, double>& unitX,
                                 vector<TreeNodeVisual>& outNodes,
                                 vector<Edge>& outEdges,
                                 map<Node*, int>& nodeIndex,
                                 int depth = 0)
{
    if (!node) return;

    // get unit (fallback to 0 if missing)
    auto it = unitX.find(node);
   double unit = (it != unitX.end()) ? it->second : 0;

    // pixel coords
    double px = unit * H_UNIT;
    int py = depth * V_UNIT;

    // build label from node's token — adapt if your field names differ
    string label;   label = trim(node->token.stringVal);
    string shape= node->shape;

    int myIndex = static_cast<int>(outNodes.size());

    nodeIndex[node] = myIndex;

    if(node->token.type == ASSIGN ) {
        label+= " ("+node->children[0]->token.stringVal+")";
        outNodes.emplace_back(px, py, label, shape);

        auto ch = node->children[1];
        collectNodesAndEdges(ch, unitX, outNodes, outEdges, nodeIndex, depth + 1);
        outEdges.emplace_back(myIndex, nodeIndex[ch], false);

    } else if(node->token.type==READ) {
        label+= " ("+node->children[0]->token.stringVal+")";
        outNodes.emplace_back(px, py, label, shape);

    } else {
        if(node->token.type == NUMBER)
            label="const("+node->token.stringVal+")";
        else if (node->token.type == IDENTIFIER)
            label="id("+node->token.stringVal+")";

        outNodes.emplace_back(px, py, label, shape);

        // Recurse children, then add edges parent->child (indices)
        for (auto* ch : node->children) {
            collectNodesAndEdges(ch, unitX, outNodes, outEdges, nodeIndex, depth + 1);
            outEdges.emplace_back(myIndex, nodeIndex[ch], false);
        }
    }

    // handle sibling 'next' pointers: draw horizontal sibling edges
    if (node->sibling) {
        collectNodesAndEdges(node->sibling, unitX, outNodes, outEdges, nodeIndex, depth);
        outEdges.emplace_back(myIndex, nodeIndex[node->sibling], true);
    }
}

// -------------------- Zoomable view (mouse wheel zoom) --------------------
class ZoomableView : public QGraphicsView {
public:
    ZoomableView(QGraphicsScene* s, QWidget* p=nullptr) : QGraphicsView(s, p) {
        setRenderHint(QPainter::Antialiasing);
        setDragMode(QGraphicsView::ScrollHandDrag);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
protected:
    void wheelEvent(QWheelEvent* e) override {
        const double factor = 1.15;
        if (e->angleDelta().y() > 0) scale(factor, factor);
        else scale(1.0/factor, 1.0/factor);
    }
};

// -------------------- Drawing helpers --------------------

// --- Replace drawNode with this (keeps node.shape but unifies style) ---
// --- unified color for all nodes (keeps shape) ---
static QGraphicsItem* drawNode(QGraphicsScene* scene,
                               double x, int y, int w, int h,
                               const QString& label, const std::string& shape,
                               const std::string& /*tokenType*/ = "")
{
        QRectF rect(x, y, w, h);

    // subtle shadow (kept)
    QBrush shadowBrush(QColor(0,0,0,30));
    QPen noPen(Qt::NoPen);
    if(shape=="ellipse")
    {
        QGraphicsEllipseItem* shadow = scene->addEllipse(rect.translated(4,4), noPen, shadowBrush);
        shadow->setZValue(0);
    }
    else {
        QGraphicsRectItem *shadow = scene->addRect(rect.translated(4, 4), noPen, shadowBrush);
        shadow->setZValue(0);
    }
    // <- ALL NODES use this exact fill color now ->

    QBrush brush(Qt::white);
    QPen pen(Qt::black);
    pen.setWidthF(1.0);

    QGraphicsItem* mainItem = nullptr;

    if (shape == "ellipse") {


        QGraphicsEllipseItem* e = scene->addEllipse(rect, pen, brush);
        e->setZValue(1);
        mainItem = e;
    } else {
        QGraphicsPathItem* r = new QGraphicsPathItem();
        QPainterPath p;
        constexpr qreal radius = 8.0;
        p.addRoundedRect(rect, radius, radius);
        r->setPath(p);
        r->setPen(pen);
        r->setBrush(brush);
        scene->addItem(r);
        r->setZValue(1);
        mainItem = r;
    }

    // consistent text placement
    QGraphicsTextItem* text = scene->addText(label);
    QFont f = text->font();
    int fontSize = std::max(10, NODE_H / 3);
    f.setPointSize(fontSize);
    text->setFont(f);
    text->setTextWidth(w - 12); // padding

    QRectF tr = text->boundingRect();
    tr = text->boundingRect(); // recompute after width set
    qreal tx = x + (w - tr.width()) / 2.0;
    qreal ty = y + (h - tr.height()) / 2.0;
    text->setPos(tx, ty);
    text->setDefaultTextColor(Qt::black);

    text->setParentItem(mainItem);
    return mainItem;
}

// Draw straight edge (parent bottom center -> child top center)
// Replace old drawEdge with this sibling-aware implementation
// --- robust edge drawing: polyline for parent->child, pure horizontal for siblings ---
// --- Replace drawEdge with this: straight parent->child lines, horizontal sibling lines ---
static void drawEdge(QGraphicsScene* scene, const TreeNodeVisual& p, const TreeNodeVisual& c, bool isSibling) {
    QPen pen(Qt::black);
    pen.setWidthF(1.0);

    if (isSibling) {
        // horizontal line between node centers (same depth expected)
        qreal y = p.y + NODE_H / 2.0;
        qreal x1 = p.x + NODE_W / 2.0;
        qreal x2 = c.x + NODE_W / 2.0;
        scene->addLine(x1, y, x2, y, pen)->setZValue(0.6);

        // optional short vertical ticks to visually connect to node bodies
        scene->addLine(x1, y, x1, p.y + NODE_H, pen)->setZValue(0.55);
        scene->addLine(x2, y, x2, c.y + NODE_H, pen)->setZValue(0.55);
    } else {
        // straight line from parent's bottom-center to child's top-center
        QPointF p1(p.x + NODE_W / 2.0, p.y + NODE_H); // parent bottom center
        QPointF p2(c.x + NODE_W / 2.0, c.y);           // child top center
        scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen)->setZValue(0.5);
    }
}



// -------------------- Main API --------------------

int visualizeTree(Node* root, QWidget* parent)
{
    if (!root) return 1;  // nothing to do

    // 1) compute unit positions
    std::map<Node*, double> unitX;
    double leafCounter = 0;
    computeUnitX(root, unitX, leafCounter);
    vector<Edge> edges;
    // 2) collect drawable nodes & edges
    std::vector<TreeNodeVisual> nodes;

    std::map<Node*, int> nodeIndex;
    collectNodesAndEdges(root, unitX, nodes, edges, nodeIndex);

    // 3) create dialog & layout
    QDialog dialog(parent);
    dialog.setWindowTitle("Syntax Tree");
    dialog.resize(1200, 800);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // 4) scene & view (zoomable)
    QGraphicsScene* scene = new QGraphicsScene(&dialog);
    scene->setBackgroundBrush(QColor("#f6f7fb"));
    ZoomableView* view = new ZoomableView(scene, &dialog);
    view->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(view);

    // 5) draw edges first


    for (const auto& e : edges) {
        const TreeNodeVisual& p = nodes[e.from];
        const TreeNodeVisual& c = nodes[e.to];
        drawEdge(scene, p, c, e.isSibling);
    }

    // 6) draw nodes (use token.type for color if available)
    for (const auto& n : nodes) {
        // extract tokenType from label heuristic "(type)" if present
        std::string tokenType;
        auto pos1 = n.label.find('(');
        auto pos2 = n.label.rfind(')');
        if (pos1 != string::npos && pos2 != string::npos && pos2 > pos1) {
            tokenType = trim(n.label.substr(pos1+1, pos2-pos1-1));
        }
        drawNode(scene, n.x, n.y, NODE_W, NODE_H, QString::fromStdString(n.label), n.shape, tokenType);
    }

    // 7) Controls: Fit / Zoom In / Zoom Out / OK
    QHBoxLayout* buttons = new QHBoxLayout();
    QPushButton* fitBtn = new QPushButton("Fit");
    QPushButton* zoomIn = new QPushButton("+");
    QPushButton* zoomOut = new QPushButton("-");
    QPushButton* okBtn  = new QPushButton("OK");

    buttons->addWidget(fitBtn);
    buttons->addWidget(zoomIn);
    buttons->addWidget(zoomOut);
    buttons->addStretch();
    buttons->addWidget(okBtn);
    layout->addLayout(buttons);

    // connections
    QObject::connect(zoomIn,  &QPushButton::clicked, [view](){ view->scale(1.2, 1.2); });
    QObject::connect(zoomOut, &QPushButton::clicked, [view](){ view->scale(1.0/1.2, 1.0/1.2); });
    QObject::connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(fitBtn, &QPushButton::clicked, [view, scene](){
        QRectF bounds = scene->itemsBoundingRect();
        if (bounds.isNull() || bounds.width() <= 0 || bounds.height() <= 0) return;
        QSizeF vp = view->viewport()->size();
        double sx = vp.width()  / bounds.width();
        double sy = vp.height() / bounds.height();
        double targetScale = std::min(sx, sy);
        // do not shrink below 1.0
        if (targetScale < 1.0) targetScale = 1.0;
        view->resetTransform();
        view->scale(targetScale, targetScale);
        view->centerOn(bounds.center());
    });

    // 8) safer initial fit: fit but never shrink below 1.0
    {
        QRectF bounds = scene->itemsBoundingRect();
        if (!bounds.isNull() && bounds.width() > 0 && bounds.height() > 0) {
            QSizeF vp = view->viewport()->size();
            double sx = vp.width()  / bounds.width();
            double sy = vp.height() / bounds.height();
            double targetScale = std::min(sx, sy);
            if (targetScale < 1.0) targetScale = 1.0;
            view->resetTransform();
            view->scale(targetScale, targetScale);
            view->centerOn(bounds.center());
        }
    }

    // 9) show dialog
    dialog.exec();
    return 0;
}