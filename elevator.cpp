#include "elevator.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "imgui.h"
#include "imgui_internal.h"


const auto MAX_FLOORS = 12;
const auto MAX_CAP = 600.0f;
const auto PASSANGER_W = 70.0f;
const auto MAX_PASS = int(MAX_CAP / PASSANGER_W);


void Elevator::draw()
{
    elevatorGo();

    drawElevator();
    drawMenu();
}

//
void Elevator::drawQueueTypeWidgets()
{
    ImGui::PushItemWidth(360);
    ImGui::LabelText("##l1", "Rodzaj kolejki przy wybieraniu kolejnych pieter");
    ImGui::RadioButton("Kolejnosc wcisniecia", &m_queueType, 0);
    ImGui::RadioButton("Uwzglednij kierunek ruchu", &m_queueType, 1);
    ImGui::NewLine();
    ImGui::Separator();
}


void Elevator::drawPassangersWidgets(int& nextFloor)
{
    ImGui::PushItemWidth(90);
    ImGui::InputInt("Liczba pasazerow", &m_incomingPassangers);
    ImGui::InputInt("Pietro wejscia", &m_inFloor);
    ImGui::InputInt("Pietro wyjscia", &m_outFloor);

    m_incomingPassangers = std::clamp(m_incomingPassangers, 0, 32);
    m_inFloor = std::clamp(m_inFloor, 0, m_floors);
    m_outFloor = std::clamp(m_outFloor, 0, m_floors);

    if (ImGui::Button("Dodaj oczekujacach pasazerow##btn"))
    {
        if (m_inFloor != m_outFloor)
        {
            for (int i = 0; i < m_incomingPassangers; ++i)
                m_waitingPassangers.push_back({ m_inFloor, m_outFloor });
            nextFloor = m_inFloor;
        }
    }
    ImGui::NewLine();
    ImGui::Separator();
}
//
void Elevator::drawFloorsWidgets()
{
    ImGui::PushItemWidth(90);
    ImGui::InputInt("Liczba pieter", &m_floors);
    m_floors = std::clamp(m_floors, 1, MAX_FLOORS);
    ImGui::NewLine();
    ImGui::Separator();
}
//
void Elevator::drawStatInfoWidgets()
{
    ImGui::PushItemWidth(360);
    ImGui::LabelText("##l3", "Kolejne pietra: [%s]", m_queueStr.c_str());
    ImGui::PushItemWidth(360);
    ImGui::LabelText("##l4", "Liczba pasazerow w windzie: %d", int(m_passangers.size()));
}

//
void Elevator::drawResetWidtgets()
{
    if (ImGui::Button("Reset symulacji"))
    {
        m_currentFloor = 0;
        m_direction = 0;
        m_queueStr.clear();
        m_passangers.clear();
        m_queue.clear();
        m_waitingPassangers.clear();
    }
    if (ImGui::Button("Reset do wartosci domyslnych"))
    {
       
        m_currentFloor = 0;
        m_direction = 0;
        m_floors = 3;
        m_inFloor = 0;
        m_outFloor = 0;
        m_incomingPassangers = 1;
        m_outcomingPassangers = 0;
        m_queueType = 0;
        m_timewait = 4000;
        m_queueStr.clear();
        m_passangers.clear();
        m_queue.clear();
        m_waitingPassangers.clear();
    }
    ImGui::NewLine();
    ImGui::Separator();
}

//
void Elevator::drawOverloadInfo()
{
    if (m_passangers.size() > MAX_PASS)
    {
        ImGui::PushItemWidth(360);
        ImGui::LabelText("##l5", "Dopuszczalna waga przekroczona: %d/%d kg!",
            int(m_passangers.size() * PASSANGER_W), int(MAX_CAP));
        ImGui::PushItemWidth(90);
        ImGui::InputInt("Wysadz pasazerow", &m_outcomingPassangers);
        m_outcomingPassangers = std::clamp(m_outcomingPassangers, 0, 32);
        if (ImGui::Button("Wysadz"))
        {
            m_outcomingPassangers = std::min(m_outcomingPassangers, int(m_passangers.size()));
            m_passangers.erase(m_passangers.begin(), m_passangers.begin() + m_outcomingPassangers);
        }
    }
}

//
void Elevator::drawMenu()
{
    auto nextFloor = -1;
    if (!ImGui::Begin("Ustawienia"))
        return;

    drawQueueTypeWidgets();
    drawPassangersWidgets(nextFloor);
    drawFloorsWidgets();
    drawStatInfoWidgets();
    drawResetWidtgets();
    drawOverloadInfo();

    ImGui::End();

    if (nextFloor != -1)
        insertNextFloor(nextFloor);
}

//
void Elevator::insertNextFloor(int nextFloor)
{
    const auto it = std::find(m_queue.begin(), m_queue.end(), nextFloor);
    if (it != m_queue.end())
        return;

    if (m_queue.empty())
    {
        m_queue.push_back(nextFloor);
    }
    else if (m_queueType == 0)
    {
        m_queue.push_back(nextFloor);
    }
    else if (m_queueType == 1)
    {
        insertIncreasingDeacrising(nextFloor);
    }
    recalculateQueue();
}
//
void Elevator::insertIncreasingDeacrising(int nextFloor)
{
    int firstFloor;
    int secondFloor = m_currentFloor;

    for (unsigned i = 0; i < m_queue.size(); ++i)
    {
        firstFloor = secondFloor;
        secondFloor = m_queue[i];

        if ((firstFloor > nextFloor && nextFloor > secondFloor) ||
            (firstFloor < nextFloor && nextFloor < secondFloor))
        {
            m_queue.insert(m_queue.begin() + i, nextFloor);
            return;
        }
    }
    m_queue.push_back(nextFloor);
}

const auto W = 50.f;  
const auto H = 20.f;  
const auto L = 150.f; 
const auto Lh = 115.f; 
const auto Ll = 30.f;  

void addFloorPoints(std::vector<ImVec2>& points, int floor, int floors)
{
    if (floor == floors)
    {
        points.push_back(ImVec2{ W + L, H + floor * Lh });
        points.push_back(ImVec2{ W, H + floor * Lh });
        return;
    }
    
    points.push_back(ImVec2{ W + L, H + floor * Lh });
    points.push_back(ImVec2{ W + L - Ll, H + floor * Lh });
    points.push_back(ImVec2{ W + L, H + floor * Lh });

    
    addFloorPoints(points, floor + 1, floors);

    
    points.push_back(ImVec2{ W, H + floor * Lh });
    points.push_back(ImVec2{ W + Ll, H + floor * Lh });
    points.push_back(ImVec2{ W, H + floor * Lh });
}

void Elevator::drawElevator()
{
    const auto COLOR = ImU32{ 0xFF000000 };   // ARGB czarny
    const auto ELEVATOR_COLOR = ImU32{ 0xFF00FF00 };   // kolor zielony
    const auto ELEVATOR_COLOR_RED = ImU32{ 0xFF0000FF };   // kolor czerwony
    const auto ELEVATOR_COLOR_NOT_MOVE = ImU32{ 0xFF06E0E0 };   // kolor szary
    const auto TH = 10.0f; //gruboœæ linii

    static std::vector<ImVec2> shaftPoints = {};
    static int lastTimeFloors = 0;

    if (lastTimeFloors != m_floors)
    {
        lastTimeFloors = m_floors;

        std::vector<ImVec2> points = { ImVec2{W, H}, ImVec2{W + L, H} };

        addFloorPoints(points, 1, m_floors + 1);

        points.push_back(ImVec2{ W, H + Lh });
        points.push_back(ImVec2{ W + Ll, H + Lh });
        points.push_back(ImVec2{ W, H + Lh });

        std::swap(points, shaftPoints);
    }

    // rysuje winde - prostokat
    const auto E_WB = W;
    const auto E_WE = W + L;

    auto E_HB = float(H) + float(Lh) * (float(m_floors) - m_currentFloor);
    auto E_HE = float(H) + float(Lh) * (float(m_floors) - m_currentFloor + 1.0f);

    auto upperLeft = ImVec2{ E_WB, E_HB };
    auto lowerRight = ImVec2{ E_WE, E_HE };

    auto color = ELEVATOR_COLOR;

    //gdy przeci¹¿enie 
    if (m_passangers.size() > MAX_PASS)
        color = ELEVATOR_COLOR_RED;
    else if (m_direction == 0)
        color = ELEVATOR_COLOR_NOT_MOVE;

    //dodanie windy do t³a
    ImGui::GetBackgroundDrawList()->AddRectFilled(upperLeft, lowerRight, color);

    //dodanie szybu windy do t³a
    ImGui::GetBackgroundDrawList()->AddPolyline(shaftPoints.data(),
        shaftPoints.size(),
        COLOR,
        ImDrawFlags_Closed,
        TH);
}


void Elevator::elevatorGo()
{
    const auto MOVE_TIME = 100;  // Czas trwania pojedynczego ruchu windy

    static auto timestamp = std::chrono::system_clock::now();  // Pocz¹tkowy znacznik czasu

    const auto currentTime = std::chrono::system_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - timestamp).count(); 

    if (delta < m_timewait)
        return;  

    timestamp = std::chrono::system_clock::now();  

    if (m_passangers.size() > MAX_PASS)
        return;  

    if (m_direction == 1)
    {
        m_currentFloor += 0.05f;  
        m_timewait = MOVE_TIME;   
    }
    else if (m_direction == -1)
    {
        m_currentFloor -= 0.05f;  
        m_timewait = MOVE_TIME;   
    }

    
    if (!m_queue.empty() && std::fabs(m_currentFloor - float(m_queue.front())) < 0.001f)
    {
        const int currentFloor = m_queue.front();

        // Usuñ pasa¿erów, którzy wysiadaj¹ na danym piêtrze
        if (!m_passangers.empty())
            m_passangers.erase(std::remove(m_passangers.begin(),
                m_passangers.end(), currentFloor), m_passangers.end());

        // Dodaj pasa¿erów, którzy czekaj¹ na danym piêtrze do windy i do kolejki
        if (!m_waitingPassangers.empty())
        {
            for (unsigned i = 0; i < m_waitingPassangers.size(); ++i)
            {
                if (m_waitingPassangers[i].first == currentFloor)
                {
                    m_passangers.push_back(m_waitingPassangers[i].second);
                    insertNextFloor(m_waitingPassangers[i].second);
                }
            }
            m_waitingPassangers.erase(std::remove_if(m_waitingPassangers.begin(), m_waitingPassangers.end(),
                [&](const auto& passanger) { return currentFloor == passanger.first; }),
                m_waitingPassangers.end());
        }

       
        m_queue.erase(m_queue.begin());
        m_direction = 0;
        m_timewait = 2000;
        recalculateQueue();
        return;
    }

  
    if (m_direction == 0)
    {
        
        if (!m_queue.empty())
        {
            auto nextFloor = m_queue.front();
           
            if (std::fabs(m_currentFloor - float(nextFloor)) < 0.001f)
            {
                m_queue.erase(m_queue.begin());
                recalculateQueue();
            }
           
            else if (m_currentFloor < nextFloor)
            {
                m_direction = 1;
            }
            
            else
            {
                m_direction = -1;
            }
            m_timewait = MOVE_TIME;  
        }
        
        else if (std::fabs(m_currentFloor) < 0.01f)
        {
            m_currentFloor = 0.0f;
            m_timewait = 500;  
        }

        else
        {
            m_timewait = 10000;  
        }

    }
}


void Elevator::recalculateQueue()
{
    m_queueStr.clear();

    if (!m_queue.empty())
        m_queueStr += std::to_string(m_queue.front());

    for (unsigned i = 1; i < m_queue.size(); ++i)
    {
        m_queueStr += ", ";
        m_queueStr += std::to_string(m_queue[i]);
    }
}
