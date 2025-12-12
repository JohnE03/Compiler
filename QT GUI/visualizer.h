//
// Created by kiro3 on 12/11/2025.
//

#ifndef COMPILER_VISUALIZER_H
#define COMPILER_VISUALIZER_H

#include <QWidget>
#include "../Compiler Logic/Node.h"



constexpr int H_UNIT_DEFAULT = 150;
constexpr int V_UNIT_DEFAULT = 120;
constexpr int NODE_W_DEFAULT = 120;
constexpr int NODE_H_DEFAULT = 56;


int visualizeTree(Node* root, QWidget* parent = nullptr);




#endif //COMPILER_VISUALIZER_H
