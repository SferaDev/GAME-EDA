#include "Player.hh"

#define PLAYER_NAME Dana99

struct PLAYER_NAME : public Player {

    static Player *factory() {
        return new PLAYER_NAME;
    }

    typedef vector <vector<bool>> M;
    vector <Dir> knightMoves;
    vector <Dir> farmerMoves;

    virtual void play() {
        if (round() == 0) {
            knightMoves = {Top, Bottom, Left, Right, BR, LB, TL, RT};
            farmerMoves = {Top, Bottom, Left, Right};
        }

        M witchesMap(rows(), vector<bool>(cols(), false));
        M knightsMap(rows(), vector<bool>(cols(), false));
        M objectives(rows(), vector<bool>(cols(), false));
        initBoard(witchesMap, knightsMap);

        vector<int> mFarmers = farmers(0);
        for (int id : mFarmers) {
            strategyForUnit(id, witchesMap, knightsMap, objectives);
        }

        vector<int> mKnights = knights(0);
        for (int id : mKnights) {
            strategyForUnit(id, witchesMap, knightsMap, objectives);
        }

        vector<int> mWitches = witches(0);
        for (int id : mWitches) {
            strategyForUnit(id, witchesMap, knightsMap, objectives);
        }
    }

    void strategyForUnit(int id, const M &witchesMap, const M &knightsMap, M &objectives) {
        Pos unitPos = unit(id).pos;
        UnitType unitType = unit(id).type;

        Dir nextDir = calculatePath(id, unitPos, witchesMap, knightsMap, objectives);
        Pos nextPos = unitPos + nextDir;
        if (nextDir != None) command(id, nextDir);
        else {
            nextDir = Dir(random(0, unitType == Knight ? 7 : 3) * (unitType == Knight ? 1 : 2));
            nextPos = unitPos + nextDir;
            int i = 0;
            for (; i < 25 and !pos_ok(nextPos); ++i) {
                nextDir = Dir(random(0, unitType == Knight ? 7 : 3) * (unitType == Knight ? 1 : 2));
                nextPos = unitPos + nextDir;
            }
            command(id, i == 25 ? None : nextDir);
        }
        objectives[nextPos.j][nextPos.i] = true;
    }

    Dir calculatePath(int id, Pos start, const M &witchesMap, const M &knightsMap, const M &objectives) {
        return bfsAlgorithm(start, id, witchesMap, knightsMap, objectives);
    }

    Dir bfsAlgorithm(Pos start, int id, const M &witchesMap, const M &knightsMap, const M &objectives) {
        UnitType unitType = unit(id).type;
        queue < pair < Pos, pair < Pos, Dir >> > Q;
        Q.push(make_pair(start, make_pair(Pos(-1, -1), None)));
        M visited(rows(), vector<bool>(cols(), false));
        for (int iteration = 0; !Q.empty() and iteration < 500; ++iteration) {
            pair <Pos, pair<Pos, Dir>> itemPair = Q.front();
            Pos currentPos = itemPair.first;
            Q.pop();
            if (currentPos != start and isUnitGoal(currentPos, id)) return itemPair.second.second;
            else {
                if (unitType == Knight) random_shuffle(knightMoves.begin(), knightMoves.end());
                else random_shuffle(farmerMoves.begin(), farmerMoves.end());
                for (unsigned int i = 1; i <= (unitType == Knight ? knightMoves.size() : farmerMoves.size()); ++i) {
                    Dir nextDir;
                    if (unitType == Knight) nextDir = knightMoves[knightMoves.size() - i];
                    else nextDir = farmerMoves[farmerMoves.size() - i];
                    Pos nextPos = currentPos + nextDir;
                    if (isValidCell(nextPos, unitType, witchesMap, knightsMap)
                        and !visited[nextPos.j][nextPos.i] and !objectives[nextPos.j][nextPos.i] and
                        (itemPair.second.first == Pos(-1, -1) or nextPos != itemPair.second.first)) {
                        visited[nextPos.j][nextPos.i] = true;
                        if (itemPair.second.second == None) Q.push(make_pair(nextPos, make_pair(currentPos, nextDir)));
                        else Q.push(make_pair(nextPos, make_pair(currentPos, itemPair.second.second)));
                    }
                }
            }
        }
        return None;
    }

    bool isValidCell(Pos cellPos, UnitType unitType, const M &witchesMap, const M &knightsMap) {
        if (cellPos.j < 1 or cellPos.j >= cols() - 1) return false;
        if (cellPos.i < 1 or cellPos.i >= rows() - 1) return false;
        if (!pos_ok(cellPos)) return false;
        if (unitType != Witch and isHauntedCell(cellPos, witchesMap)) return false;
        if (unitType == Farmer and isThreatenedCell(cellPos, knightsMap)) return false;
        if (unitType == Farmer and cell(cellPos).id != -1) return false;
        if (cell(cellPos).type == Wall) return false;
        return true;
    }

    bool isHauntedCell(Pos cellPos, const M &witchesMap) {
        return witchesMap[cellPos.i][cellPos.j];
    }

    bool isThreatenedCell(Pos cellPos, const M &knightsMap) {
        return knightsMap[cellPos.i][cellPos.j];
    }

    bool isUnitGoal(Pos cellPos, int id) {
        switch (unit(id).type) {
            case Witch:
                return cell(cellPos).id != -1 and unit(cell(cellPos).id).player != 0
                       and unit(cell(cellPos).id).type == Knight;
            case Knight:
                return cell(cellPos).id != -1 and unit(cell(cellPos).id).player != 0
                       and unit(cell(cellPos).id).type != Witch;
            case Farmer:
                return cell(cellPos).owner != 0;
            default:
                return false;
        }
    }

    void initBoard(vector <vector<bool>> &witchesMap, vector <vector<bool>> &knightsMap) {
        for (int i = 0; i < rows(); ++i) {
            for (int j = 0; j < cols(); ++j) {
                if (cell(i, j).type == Wall) witchesMap[i][j] = true;
                if (cell(i, j).type == Wall) knightsMap[i][j] = true;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if (i > 0) {
                vector<int> _knights = knights(i);
                for (unsigned int j = 0; j < _knights.size(); ++j) {
                    Pos tempPos = unit(_knights[j]).pos;
                    for (int x = 0; x < DirSize; ++x) {
                        Pos auxPos = tempPos + Dir(x);
                        if (pos_ok(auxPos)) knightsMap[auxPos.i][auxPos.j] = true;
                    }
                }
            }
            vector<int> _witches = witches(i);
            for (unsigned int j = 0; j < _witches.size(); ++j) {
                Pos tempPos = unit(_witches[j]).pos;
                bool active = unit(_witches[j]).active;
                for (int x = 0; x < DirSize - 1; ++x) {
                    for (int y = 0; y < 5; ++y) {
                        for (int z = 0; z < 5; ++z) {
                            Pos auxPos;
                            if (active) auxPos = tempPos + Dir(x) + Dir(y * 2) + Dir(z * 2);
                            else auxPos = tempPos + Dir(x) + Dir(y * 2);
                            if (pos_ok(auxPos)) witchesMap[auxPos.i][auxPos.j] = true;
                        }
                    }
                }
            }
        }
    }
};

RegisterPlayer(PLAYER_NAME);
