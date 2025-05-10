#include "testlib.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

const int N = 1e3 + 10;  // 用于数组大小
const int W = 500;       // 用于坐标偏移
const int MAX_COORD = 999; // 最大允许坐标绝对值

// 使用与std.cpp相似的数组结构
struct node {
  int val[N * 2][N * 2];
  int* operator [] (int x) {
    return val[x + W] + W;
  }
} w;

int n;                  // 问题规模
int player_x, player_y; // 玩家位置
int fragments_cnt;      // 碎片数量
int max_size = 1;       // 当前最大碎片大小

const int dx[4] = {-1, 1, 0, 0};  // 对应LRUD的x方向移动
const int dy[4] = {0, 0, 1, -1};  // 对应LRUD的y方向移动
const char dir[4] = {'L', 'R', 'U', 'D'};  // 方向字符

// 清空游戏状态
void clear_state() {
    for (int i = 0; i < N * 2; i++) {
        for (int j = 0; j < N * 2; j++) {
            w.val[i][j] = 0;
        }
    }
    max_size = 1;
    fragments_cnt = 0;
}

// 移动检查
bool can_move(int dir_idx) {
    int nx = player_x + dx[dir_idx];
    int ny = player_y + dy[dir_idx];
    
    // 目标位置没有碎片，可以直接移动
    if (w[nx][ny] == 0) {
        return true;
    }
    
    // 目标位置有碎片，检查是否可以推动或合并
    int nnx = nx + dx[dir_idx];
    int nny = ny + dy[dir_idx];
    
    // 推动：目标位置有碎片，目标后面空位
    if (w[nx][ny] > 0 && w[nnx][nny] == 0) {
        return true;
    }
    
    // 合并：目标位置有大小为k的碎片，目标后面有同样大小的碎片
    if (w[nx][ny] > 0 && w[nx][ny] == w[nnx][nny]) {
        return true;
    }
    
    return false;
}

// 执行移动
void do_move(int dir_idx) {
    int nx = player_x + dx[dir_idx];
    int ny = player_y + dy[dir_idx];
    
    // 移动到空位置
    if (w[nx][ny] == 0) {
        player_x = nx;
        player_y = ny;
        return;
    }
    
    // 推动或合并碎片
    int nnx = nx + dx[dir_idx];
    int nny = ny + dy[dir_idx];
    
    if (w[nnx][nny] == 0) {
        // 推动
        w[nnx][nny] = w[nx][ny];
        w[nx][ny] = 0;
    } else {
        // 合并
        w[nnx][nny] *= 2;
        w[nx][ny] = 0;
        fragments_cnt--;
        max_size = max(max_size, w[nnx][nny]);
    }
    
    player_x = nx;
    player_y = ny;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    
    // 读取输入
    n = inf.readInt(1, 16);
    int target_size = 1 << n;
    
    player_x = inf.readInt(1, 300);
    player_y = inf.readInt(1, 300);
    
    clear_state();
    fragments_cnt = (1 << n);
    
    // 读取碎片位置
    for (int i = 0; i < fragments_cnt; i++) {
        int x = inf.readInt(1, 300);
        int y = inf.readInt(1, 300);
        w[x][y] = 1;  // 初始碎片大小为1
    }
    
    // 读取参赛者输出的第一行
    string result_line = ouf.readLine();
    
    // 判断是否输出了LUCKY（在这个问题中我们知道一定有解）
    if (result_line == "LUCKY") {
        quitf(_wa, "Incorrectly claimed no solution exists");
    } else if (result_line != "SILKSONG") {
        quitf(_wa, "Expected 'SILKSONG' or 'LUCKY' on first line, got '%s'", result_line.c_str());
    }
    
    // 读取移动序列
    string moves = ouf.readLine();
    
    // 检查移动次数是否超过限制
    if (moves.length() > 3000000) {
        quitf(_wa, "Too many moves: %d", (int)moves.length());
    }
    
    // 模拟每一步移动
    for (size_t i = 0; i < moves.length(); i++) {
        int dir_idx = -1;
        for (int j = 0; j < 4; j++) {
            if (moves[i] == dir[j]) {
                dir_idx = j;
                break;
            }
        }
        
        if (dir_idx == -1) {
            quitf(_wa, "Invalid move character: %c", moves[i]);
        }
        
        if (!can_move(dir_idx)) {
            quitf(_wa, "Invalid move at step %d: %c", (int)i + 1, moves[i]);
        }
        
        do_move(dir_idx);
        
        // 检查坐标是否在允许范围内
        if (abs(player_x) > MAX_COORD || abs(player_y) > MAX_COORD) {
            quitf(_wa, "Player coordinates exceed the limit of %d at step %d", MAX_COORD, (int)i + 1);
        }
    }
    
    // 检查是否达成目标
    if (max_size == target_size) {
        quitf(_ok, "Valid solution with %d moves", (int)moves.length());
    } else {
        quitf(_wa, "Failed to create a fragment of size %d (got %d)", target_size, max_size);
    }
    
    return 0;
}