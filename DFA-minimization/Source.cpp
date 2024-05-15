#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "DFA.cpp"
#include "Source.h"
using namespace std;
using namespace sf;

// Sorting Criteria for vector
// - first to sort according to flags [ non-accepting states first, then accepting states]
// - second to sort according to statediff(I, II, III) [accending order]
// - thirdly to sort according to the names of the state(q0, q1) [accending order]

bool nameandflag(const pair<string, DFAclass>& a, const pair<string, DFAclass>& b)
{
    if (a.second.flag != b.second.flag)
    {
        return !a.second.flag && b.second.flag;
    }
    else if (a.second.statediff != b.second.statediff)
    {
        return a.second.statediff < b.second.statediff;
    }
    else
    {
        return a.first < b.first;
    }
}

// roman function returns a roman number for statediff string variable

string roman(int num)
{
    string ans;

    while (num != 0)
    {
        if (num >= 10)
        {
            ans += "X";

            num -= 10;
        }
        else if ((num + 1) % 10 == 0)
        {
            ans += "IX";

            num -= 9;
        }
        else if ((num + 1) % 5 == 0)
        {
            ans += "IV";

            num -= 4;
        }
        else if (num >= 5)
        {
            ans += "V";

            num -= 5;
        }
        else
        {
            ans += "I";
            num -= 1;
        }
    }
    return ans;
}

// forreturnstring iterates over the transition vector of all the states stores it in a string
// and returns it

string forreturnstring(vector<pair<string, DFAclass>> mini)
{
    string ans;

    for (const auto& p1 : mini)
    {
        for (const auto it : p1.second.transitions)
        {
            ans += it;

            ans += ", ";
        }
    }
    return ans;
}

// checks if the state is present in the vector then return the attributes of that state

pair<string, DFAclass> find(string f, vector<pair<string, DFAclass>> mini)
{
    for (const auto fin : mini)
    {
        if (f == fin.first)
        {
            return fin;
        }
    }

    return {};
}

int statecounts = 2;

vector<string> set = { "a", "b", "c" };

vector<pair<string, vector<string>>> finalprinting;

vector<pair<string, pair<vector<string>, bool>>> MINIMIZATION(unordered_map<string, DFAclass> minimization, int numberofstates, int variables)
{
    unordered_map<string, DFAclass> m;
    // copies the unordered map provided as the argument of the function in a new unordered map
    // due to the limitations of SFML as it fills the unordered map with empty spaces that 
    // causes problems with the minimization algorithm

    for (auto& pair : minimization)
    {
        vector<string> tempp;

        int u = variables;

        for (int j = (pair.second.states.size() - (u)); j < pair.second.states.size(); j++)
        {   
            tempp.push_back(pair.second.states[j]);
        }

        pair.second.states = tempp;

        m.insert( { pair.first, pair.second } );
    }

    // Allocate the roman representation to the states

    for (auto& pair : m)
    {
        if (pair.second.flag)
        {
            pair.second.statediff = roman(2);
        }
        else
        {
            pair.second.statediff = roman(1);
        }
    }

    // Fills the transition vector 

    for (auto& pair : m)
    {
        for (int j = 0; j < pair.second.states.size(); j++)
        {
            string t = pair.second.states[j];
            auto find = m.find(t);
            pair.second.transitions[j] = find->second.statediff;  
        }
    }

    // copying the unordered map into a vector

    vector<pair<string, DFAclass>> mini(m.begin(), m.end());

    // sorting the vector according to the sorting criteria provided as a function(nameandflag)

    sort(mini.begin(), mini.end(), nameandflag);

    string prev = forreturnstring(mini);

    string curr;

    pair <string, DFAclass> search;

    // the reason of keeping the following condition for the while loop is the transition vector
    // will not be the same of the vector until and unless the provided dfa is minimized

    while (prev != curr)
    {
        // comparing if the states have the same roman representation and flag status
        // if the transition vector is not same of the neighbouring states then 
        // then they should be not have same roman representation 

        for (int i = 0; i < mini.size() - 1; i++)
        {
            if (mini[i].second.statediff == mini[i + 1].second.statediff)
            {
                if (!mini[i].second.flag && !mini[i + 1].second.flag)
                {
                    if (mini[i].second.transitions != mini[i + 1].second.transitions)
                    {
                        statecounts++;

                        int temp = i + 1;

                        mini[i + 1].second.statediff = roman(statecounts);

                        while (temp < mini.size() - 1 && mini[temp].second.transitions == mini[temp + 1].second.transitions)
                        {
                            if (mini[temp].second.flag == mini[temp + 1].second.flag)
                            {
                                mini[temp + 1].second.statediff = roman(statecounts);
                                temp++;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }

        // same loop as above but since the flag status is different so a different loop

        for (int i = 0; i < mini.size() - 1; i++)
        {
            if (mini[i].second.statediff == mini[i + 1].second.statediff)
            {
                if (mini[i].second.flag && mini[i + 1].second.flag)
                {
                    if (mini[i].second.transitions != mini[i + 1].second.transitions)
                    {
                        statecounts++;

                        int temp = i + 1;

                        mini[i + 1].second.statediff = roman(statecounts);

                        while (temp < mini.size() - 1 && mini[temp].second.transitions == mini[temp + 1].second.transitions)
                        {
                            if (mini[temp].second.flag == mini[temp + 1].second.flag)
                            {
                                mini[temp + 1].second.statediff = roman(statecounts);
                                temp++;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }

        // changing the values of the transition vector since the statediff variable is now changed

        for (auto& pair : mini)
        {
            for (int i = 0; i < pair.second.states.size(); i++)
            {
                string t;

                t = pair.second.states[i];

                search = find(t, mini);

                pair.second.transitions[i] = search.second.statediff;
            }
        }

        prev = curr;

        curr = forreturnstring(mini);

        sort(mini.begin(), mini.end(), nameandflag);
    }

    vector<pair<string, pair<vector<string>, bool>>> finalprinting;

    pair<string, pair<vector<string>, bool>> temppair;

    pair<vector<string>, bool> temppairpair;

    // if the roman representation is same then it can be combined into 1
    // so pushing back only the states with different roman representation

    for (int i = 0; i < mini.size() - 1; i++)
    {
        if (mini[i].second.statediff != mini[i + 1].second.statediff)
        {
            temppair.first = mini[i].second.statediff;

            temppairpair.first = mini[i].second.transitions;

            temppairpair.second = mini[i].second.flag;

            temppair.second = temppairpair;

            finalprinting.push_back(temppair);

        }
    }

    // last state will always be pushed back in both the cases

    temppair.first = mini[mini.size() - 1].second.statediff;

    temppairpair.first = mini[mini.size() - 1].second.transitions;

    temppairpair.second = mini[mini.size() - 1].second.flag;

    temppair.second = temppairpair;

    finalprinting.push_back(temppair);

    // sorting the vector before returning 

    sort(finalprinting.begin(), finalprinting.end());

    return finalprinting;
}

CircleShape stateadd(float x, float y)
{
    CircleShape circle;
    circle.setRadius(35);
    circle.setPosition(x, y);
    circle.setFillColor(Color::White);

    return circle;
}

CircleShape finalstateadd(float x, float y, Color color)
{
    CircleShape circle;
    circle.setRadius(40);
    circle.setPosition(x, y);
    circle.setFillColor(Color::Transparent);
    circle.setOutlineThickness(3);
    circle.setOutlineColor(color);
    return circle;
}

VertexArray borders(float x0, float y0, float x1, float y1, Color color)
{
    VertexArray border(Lines, 2);
    border[0].position = Vector2f(x0, y0);
    border[1].position = Vector2f(x1, y1);
    border[0].color = color;
    border[1].color = color;
    return border;
}

RectangleShape inputbox(float width, float height, float x, float y)
{
    RectangleShape box(Vector2f(width, height));
    box.setFillColor(Color(0, 0, 0, 128));
    box.setPosition(x, y);
    return box;
}

VertexArray linedraw(float x1, float y1, float x2, float y2)
{
    VertexArray line(Lines, 2);

    line[0].position = Vector2f(x1, y1);
    line[1].position = Vector2f(x2, y2);

    line[0].color = Color::White;
    line[1].color = Color::White;

    return line;
}

enum Page
{
    mainpage,
    stateconnection,
    finalpage,
};

bool test = false;
bool onetime = true;
bool isDrawing = false;
bool tempcheck = false;
bool finalselect = true;
bool islineactive = false;
bool finalfinalselect = false;
bool isstateinputactive = false;
bool isvariableinputactive = false;
bool isfinalstateinputactive = false;
float sp, ep;
int flag = 0;
int assign = 2;
int switchflag = 0;
int finalcount = 0;
int colorchange = 0;

int main()
{
    Font fontinsidecircle;
    Text temp("", fontinsidecircle, 20);
    Page currentpage = mainpage;
    string statename = "q";
    int statecount = 0, numberofstates = 0, variables = 0, finalstates = 0, statetrack = 0;
    vector <CircleShape> States;
    vector <CircleShape> FinalStates;
    vector <Text> StatesNames;
    float start = 50.f, end = 50.f;
    Vector2f startpos, endpos;
    VertexArray curve(LineStrip);
    vector <VertexArray> trns;
    vector<pair<string, pair<vector<string>, bool>>> finalprinting;
    vector<CircleShape> stateprint;
    vector<CircleShape> transitionprint;
    vector<CircleShape> tprint;
    vector<CircleShape> finalsprint;
    vector<Text> statenameprint;

    VideoMode desktop = VideoMode::getDesktopMode();

    unordered_map <string, DFAclass> minimization;

    fontinsidecircle.loadFromFile("Abel-Regular.ttf");
    temp.setPosition(desktop.width / 2.81, desktop.height / 2.5);

    //VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow window(VideoMode(desktop.width, desktop.height), "DFA Minimization");

    cout << "Width: " << desktop.width << "Height: " << desktop.height << endl;

    //StateName:
    Text textinsidecircle;
    textinsidecircle.setFont(fontinsidecircle);
    textinsidecircle.setString(statename + to_string(statecount));
    textinsidecircle.setCharacterSize(24);
    textinsidecircle.setFillColor(Color::Black);

    //Borders:
    VertexArray right = borders(desktop.width - 30.f, 30.0, desktop.width - 30.f, desktop.height - 30.f, Color::White);
    VertexArray left = borders(30.0, 30.0, 30.0, desktop.height - 30.f, Color::White);
    VertexArray top = borders(30.0, 30.0, desktop.width - 30.f, 30.0, Color::White);
    VertexArray bottom = borders(30.0, desktop.height - 30.f, desktop.width - 30.f, desktop.height - 30.f, Color::White);

    Text Greeting("                  Welcome to DFA Minimizer\n Please answer the below questions for your dfa", fontinsidecircle, 30);
    Greeting.setFillColor(Color::White);
    Greeting.setPosition(desktop.width / 3.87, desktop.height / 4.096);

    RectangleShape stateinput = inputbox(300.f, 40.f, desktop.width / 2.84, desktop.height / 2.56);
    Text si("Enter the number of states:(1-12) ", fontinsidecircle, 20);
    si.setFillColor(Color::White);
    si.setPosition(desktop.width / 2.81, desktop.height / 2.5);
    RectangleShape variableinput = inputbox(300.f, 40.f, desktop.width / 2.84, desktop.height / 2.56);
    Text vi("Enter the number of variables:(1-3) ", fontinsidecircle, 20);
    vi.setFillColor(Color::White);
    vi.setPosition(desktop.width / 2.81, desktop.height / 2.5);
    RectangleShape finalstateinput = inputbox(300.f, 40.f, desktop.width / 2.84, desktop.height / 2.56);
    Text fi("Enter the number of Final States: ", fontinsidecircle, 20);
    fi.setFillColor(Color::White);
    fi.setPosition(desktop.width / 2.81, desktop.height / 2.5);

    Text A("A", fontinsidecircle, 20);
    A.setFillColor(Color::White);
    A.setPosition(1100.f, 300.f);

    Text B("B", fontinsidecircle, 20);
    B.setFillColor(Color::Black);
    B.setPosition(1100.f, 350.f);

    Text C("C", fontinsidecircle, 20);
    C.setFillColor(Color::Yellow);
    C.setPosition(1100.f, 400.f);

    Text Sa("States", fontinsidecircle, 20);
    Sa.setFillColor(Color::White);

    Text Sb("States", fontinsidecircle, 20);
    Sb.setFillColor(Color::White);

    Text finalp("Select the final states:", fontinsidecircle, 20);
    finalp.setFillColor(Color::White);
    finalp.setPosition(desktop.width / 2.32, desktop.height / 20.48);
    finalp.setOutlineThickness(2);
    finalp.setCharacterSize(27);

    Text tname("Draw the Transitions: ", fontinsidecircle, 20);
    tname.setFillColor(Color::White);
    tname.setPosition(desktop.width / 2.32, desktop.height / 20.48);
    tname.setOutlineThickness(2);
    tname.setCharacterSize(27);

    while (window.isOpen())
    {
        Event ev;
        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:
                if (ev.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (ev.key.code == Keyboard::Enter)
                {
                    if (isstateinputactive && !temp.getString().isEmpty())
                    {
                        if (stoi(temp.getString().toAnsiString()) > 12)
                        {

                        }
                        else
                        {
                            isstateinputactive = false;
                            switchflag = 1;
                            States.resize(numberofstates);
                            States.clear();

                            start = desktop.width / 25.6;
                            end = desktop.height / 10.24;

                            for (int i = 0; i < numberofstates; i++)
                            {
                                if (i % 3 == 0 && i != 0)
                                {
                                    end += (desktop.height / 4.45);
                                    start = desktop.width / 25.6;
                                }
                                States.push_back(stateadd(start, end));
                                start += (desktop.width / 3.28);
                                textinsidecircle.setFont(fontinsidecircle);
                                textinsidecircle.setString(statename + to_string(statecount));
                                textinsidecircle.setCharacterSize(24);
                                textinsidecircle.setFillColor(Color::Black);
                                FloatRect textBounds = textinsidecircle.getLocalBounds();
                                textinsidecircle.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
                                textinsidecircle.setPosition(States[i].getPosition().x + States[i].getRadius(), States[i].getPosition().y + States[i].getRadius());
                                StatesNames.push_back(textinsidecircle);
                                statecount++;
                            }
                        }
                        
                        temp.setString("");
                        
                    }
                    else if (isvariableinputactive && !temp.getString().isEmpty())
                    {
                        if (stoi(temp.getString().toAnsiString()) > 3)
                        {

                        }
                        else
                        {
                            isvariableinputactive = false;
                            switchflag = 2;
                        }
                        temp.setString("");
                    }
                    else if (isfinalstateinputactive && !temp.getString().isEmpty())
                    {
                        if (stoi(temp.getString().toAnsiString()) < numberofstates)
                        {
                            isfinalstateinputactive = false;
                            
                            for (int i = 0; i < numberofstates; i++)
                            {
                                minimization["q" + to_string(i)] = DFAclass(variables);
                            }
                            currentpage = stateconnection;
                        }
                        temp.setString("");
                    }
                }
            case Event::TextEntered:
                if (currentpage == mainpage)
                {
                    if (isstateinputactive)
                    {
                        if (ev.text.unicode == 8 && !temp.getString().isEmpty())
                        {
                            temp.setString(temp.getString().substring(0, temp.getString().getSize() - 1));
                        }
                        else if (ev.text.unicode >= 48 && ev.text.unicode < 58)
                        {
                            temp.setString(temp.getString() + static_cast<char>(ev.text.unicode));
                        }
                        string sc = temp.getString().toAnsiString();
                        if (!sc.empty())
                        {
                            numberofstates = stoi(sc);
                        }
                    }
                    if (isvariableinputactive)
                    {
                        if (ev.text.unicode == 8 && !temp.getString().isEmpty())
                        {
                            temp.setString(temp.getString().substring(0, temp.getString().getSize() - 1));
                        }
                        else if (ev.text.unicode >= 48 && ev.text.unicode < 58)
                        {
                            temp.setString(temp.getString() + static_cast<char>(ev.text.unicode));
                        }
                        string sc = temp.getString().toAnsiString();

                        if (!sc.empty())
                        {
                            variables = stoi(sc);
                        }
                    }
                    if (isfinalstateinputactive)
                    {
                        if (ev.text.unicode == 8 && !temp.getString().isEmpty())
                        {
                            temp.setString(temp.getString().substring(0, temp.getString().getSize() - 1));
                        }
                        else if (ev.text.unicode >= 48 && ev.text.unicode < 58)
                        {
                            temp.setString(temp.getString() + static_cast<char>(ev.text.unicode));
                        }
                        string sc = temp.getString().toAnsiString();

                        if (!sc.empty())
                        {
                            finalstates = stoi(sc);
                        }
                    }
                }
            case Event::MouseButtonPressed:
                if (ev.mouseButton.button == Mouse::Left)
                {
                    if (currentpage == mainpage)
                    {
                        if (stateinput.getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)) && temp.getString().isEmpty() && switchflag == 0)
                        {
                            isstateinputactive = true;
                        }
                        else if (variableinput.getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)) && switchflag == 1 && temp.getString().isEmpty())
                        {
                            isvariableinputactive = true;
                        }
                        else if (finalstateinput.getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)) && temp.getString().isEmpty() && switchflag == 2)
                        {
                            isfinalstateinputactive = true;
                        }
                    }
                    else if (currentpage == stateconnection)
                    {
                        if (finalselect)
                        {
                            for (int i = 0; i < numberofstates; i++)
                            {
                                if (States[i].getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)))
                                {
                                    FinalStates.push_back(finalstateadd(States[i].getPosition().x - (desktop.width / 256), States[i].getPosition().y - (desktop.height / 204.8), Color::White));
                                    minimization["q" + to_string(i)].flag = true;
                                    finalcount++;
                                    if (finalcount == finalstates)
                                    {
                                        finalselect = false;
                                        finalfinalselect = true;
                                        finalp.setOutlineThickness(0);
                                        break;
                                    }
                                }
                            }
                        }
                        else if (finalfinalselect)
                        {
                            if (flag == 0)
                            {
                                if (States[statetrack].getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)))
                                {
                                    isDrawing = true;
                                    Vertex PO = Vertex(Vector2f(float(ev.mouseButton.x), float(ev.mouseButton.y)), Color(38, 164, 00));
                                    curve.clear();
                                    curve.append(PO);
                                }
                            }
                            else if (flag == 1)
                            {
                                if (States[statetrack].getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)))
                                {
                                    isDrawing = true;
                                    Vertex PO = Vertex(Vector2f(float(ev.mouseButton.x), float(ev.mouseButton.y)), Color(250, 45, 155));
                                    curve.clear();
                                    curve.append(PO);
                                }
                            }
                            else if (flag == 2)
                            {
                                if (States[statetrack].getGlobalBounds().contains(float(ev.mouseButton.x), float(ev.mouseButton.y)))
                                {
                                    isDrawing = true;
                                    Vertex PO = Vertex(Vector2f(float(ev.mouseButton.x), float(ev.mouseButton.y)), Color(252, 243, 00));
                                    curve.clear();
                                    curve.append(PO);
                                }
                            }
                        }
                    }
                }
                break;
            case Event::MouseButtonReleased:
                if (ev.mouseButton.button == Mouse::Left)
                {
                    if (finalfinalselect)
                    {
                        for (int i = 0; i < States.size(); i++)
                        {
                            if (States[i].getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y) && isDrawing)
                            {
                                minimization["q" + to_string(statetrack)].states.push_back("q" + to_string(i));
                                tempcheck = true;
                            }
                        }
                    }
                    if (finalfinalselect)
                    {
                        if (tempcheck)
                        {
                            trns.push_back(curve);
                            isDrawing = false;
                            tempcheck = false;
                            if (variables == 3)
                            {
                                if (flag < 2)
                                {
                                    flag++;
                                }
                                else if (flag == 2)
                                {
                                    flag = 0;
                                    if ((statetrack + 1) < numberofstates)
                                    {
                                        statetrack++;
                                    }
                                    else
                                    {
                                        currentpage = finalpage;
                                    }
                                }
                            }
                            else if (variables == 2)
                            {
                                if (flag < 1)
                                {
                                    flag++;
                                }
                                else if (flag == 1)
                                {
                                    flag = 0;
                                    if ((statetrack + 1) < numberofstates)
                                    {
                                        statetrack++;
                                    }
                                    else
                                    {
                                        currentpage = finalpage;
                                    }
                                }
                            }
                            else if (variables == 1)
                            {
                                if ((statetrack + 1) < numberofstates)
                                {
                                    statetrack++;
                                }
                                else
                                {
                                    currentpage = finalpage;
                                }
                            }
                        }
                    }
                }

            case Event::MouseMoved:
                if (finalfinalselect)
                {
                    if (isDrawing)
                    {
                        if (flag == 0)
                        {
                            Vertex RO = Vertex(Vector2f(float(ev.mouseMove.x), float(ev.mouseMove.y)), Color(38, 164, 00));
                            curve.append(RO);
                        }
                        else if (flag == 1)
                        {
                            Vertex RO = Vertex(Vector2f(float(ev.mouseMove.x), float(ev.mouseMove.y)), Color(250, 45, 155));
                            curve.append(RO);
                        }
                        else if (flag == 2)
                        {
                            Vertex RO = Vertex(Vector2f(float(ev.mouseMove.x), float(ev.mouseMove.y)), Color(252, 243, 00));
                            curve.append(RO);
                        }
                    }
                }
            }

            if (currentpage == mainpage)
            {
                window.clear(Color(02, 62, 138));
                window.draw(Greeting);
                window.draw(left);
                window.draw(right);
                window.draw(top);
                window.draw(bottom);

                if (!isstateinputactive && switchflag == 0)
                {
                    window.draw(stateinput);
                    window.draw(si);
                }
                else if (isstateinputactive && switchflag == 0)
                {
                    window.draw(stateinput);
                    window.draw(temp);
                }
                if (!isvariableinputactive && switchflag == 1)
                {
                    window.draw(variableinput);
                    window.draw(vi);
                }
                else if (isvariableinputactive && switchflag == 1)
                {
                    window.draw(variableinput);
                    window.draw(temp);
                }
                if (!isfinalstateinputactive && switchflag == 2)
                {
                    window.draw(finalstateinput);
                    window.draw(fi);
                }
                else if (isfinalstateinputactive && switchflag == 2)
                {
                    window.draw(finalstateinput);
                    window.draw(temp);
                }
            }
            else if (currentpage == stateconnection)
            {
                window.clear(Color(02, 62, 138));
                window.draw(left);
                window.draw(right);
                window.draw(top);
                window.draw(bottom);

                for (int i = 0; i < numberofstates; i++)
                {
                    window.draw(States[i]);
                    window.draw(StatesNames[i]);
                }

                for (int i = 0; i < FinalStates.size(); i++)
                {
                    window.draw(FinalStates[i]);
                }

                for (int i = 0; i < trns.size(); i++)
                {
                    window.draw(trns[i]);
                }

                if (finalfinalselect)
                {
                    window.draw(tname);
                    if (flag == 0)
                    {
                        CircleShape circletemp = finalstateadd(States[statetrack].getPosition().x - (desktop.width / 256), States[statetrack].getPosition().y - (desktop.height / 204.8), Color(38, 164, 00));
                        window.draw(circletemp);
                    }
                    else if (flag == 1)
                    {
                        CircleShape circletemp = finalstateadd(States[statetrack].getPosition().x - (desktop.width / 256), States[statetrack].getPosition().y - (desktop.height / 204.8), Color(250, 45, 155));
                        window.draw(circletemp);
                    }
                    else if (flag == 2)
                    {
                        CircleShape circletemp = finalstateadd(States[statetrack].getPosition().x - (desktop.width / 256), States[statetrack].getPosition().y - (desktop.height / 204.8), Color(252, 243, 00));
                        window.draw(circletemp);
                    }
                }
                else
                {
                    window.draw(finalp);
                }
            }
            else if (currentpage == finalpage)
            {
                window.clear(Color(02, 62, 138));
                window.draw(left);
                window.draw(right);
                window.draw(top);
                window.draw(bottom);

                if (onetime)
                {
                    onetime = false;
                    finalprinting = MINIMIZATION(minimization, numberofstates, variables);

                    start = desktop.width / 25.6, end = desktop.height / 10.24;

                    int st = 0;
                    int tt = 0;

                    for (int i = 0; i < finalprinting.size(); i++)
                    {
                        if (st == 6)
                        {
                            start = desktop.width / 1.82;
                            end = desktop.height / 10.24;
                        }
                        stateprint.push_back(stateadd(start, end));
                        if (finalprinting[i].second.second)
                        {
                            finalsprint.push_back(finalstateadd(start - (desktop.width / 256), end - (desktop.height / 204.8), Color::White));
                        }
                        start += (desktop.width / 8.53);
                        textinsidecircle.setFont(fontinsidecircle);
                        textinsidecircle.setString(finalprinting[i].first);
                        textinsidecircle.setCharacterSize(24);
                        textinsidecircle.setFillColor(Color::Black);
                        FloatRect textBounds = textinsidecircle.getLocalBounds(); textinsidecircle.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
                        textinsidecircle.setPosition(stateprint[st].getPosition().x + stateprint[st].getRadius(), stateprint[st].getPosition().y + stateprint[st].getRadius());
                        statenameprint.push_back(textinsidecircle);
                        st++;

                        for (int j = 0; j < finalprinting[i].second.first.size(); j++)
                        {
                            transitionprint.push_back(stateadd(start, end));

                            if (j == 0)
                            {
                                tprint.push_back(finalstateadd(start - (desktop.width / 256), end - (desktop.height / 204.8), Color(38, 164, 00)));
                            }
                            else if (j == 1)
                            {
                                tprint.push_back(finalstateadd(start - (desktop.width / 256), end - (desktop.height / 204.8), Color(250, 45, 155)));
                            }
                            else if (j == 2)
                            {
                                tprint.push_back(finalstateadd(start - (desktop.width / 256), end - (desktop.height / 204.8), Color(252, 243, 00)));
                            }

                            start += (desktop.width / 12.8);
                            textinsidecircle.setFont(fontinsidecircle);
                            textinsidecircle.setString(finalprinting[i].second.first[j]);
                            textinsidecircle.setCharacterSize(24);
                            textinsidecircle.setFillColor(Color::Black);
                            FloatRect textBounds = textinsidecircle.getLocalBounds();
                            textinsidecircle.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
                            textinsidecircle.setPosition(transitionprint[tt].getPosition().x + transitionprint[tt].getRadius(), transitionprint[tt].getPosition().y + transitionprint[tt].getRadius());
                            statenameprint.push_back(textinsidecircle);
                            tt++;
                        }
                        end += (desktop.height / 6.82);
                        if (st >= 6)
                        {
                            start = desktop.width / 1.82;
                        }
                        else
                        {
                            start = desktop.width / 25.6;
                        }
                        
                    }
                }

                Sa.setPosition(desktop.width / 21.33, desktop.height / 25.6);
                Sa.setOutlineThickness(1);
                A.setFillColor(Color::White);
                B.setFillColor(Color::White);
                C.setFillColor(Color::White);
                A.setPosition(desktop.width / 5.56, desktop.height / 25.6);
                B.setPosition(desktop.width / 3.87, desktop.height / 25.6);
                C.setPosition(desktop.width / 2.97, desktop.height / 25.6);

                window.draw(Sa);

                if (variables == 1)
                {
                    window.draw(A);
                }
                else if (variables == 2)
                {
                    window.draw(A);
                    window.draw(B);
                }
                else if (variables == 3)
                {
                    window.draw(A);
                    window.draw(B);
                    window.draw(C);
                }

                if (stateprint.size() > 6)
                {
                    Sb.setPosition(desktop.width / 1.80, desktop.height / 25.6);
                    Sb.setOutlineThickness(1);
                    window.draw(Sb);
                    window.draw(linedraw(stateprint[0].getPosition().x + (desktop.width / 12.8), stateprint[0].getPosition().y + stateprint[0].getRadius(), stateprint[5].getPosition().x + (desktop.width / 12.8), stateprint[5].getPosition().y + stateprint[5].getRadius()));
                    window.draw(linedraw(stateprint[0].getPosition().x + (desktop.width / 11.63), stateprint[0].getPosition().y + stateprint[0].getRadius(), stateprint[5].getPosition().x + (desktop.width / 11.63), stateprint[5].getPosition().y + stateprint[5].getRadius()));
                    window.draw(linedraw(stateprint[6].getPosition().x + (desktop.width / 12.8), stateprint[6].getPosition().y + stateprint[6].getRadius(), stateprint[stateprint.size() - 1].getPosition().x + (desktop.width / 12.8), stateprint[stateprint.size() - 1].getPosition().y + stateprint[stateprint.size() - 1].getRadius()));
                    window.draw(linedraw(stateprint[6].getPosition().x + (desktop.width / 11.63), stateprint[6].getPosition().y + stateprint[6].getRadius(), stateprint[stateprint.size() - 1].getPosition().x + (desktop.width / 11.63), stateprint[stateprint.size() - 1].getPosition().y + stateprint[stateprint.size() - 1].getRadius()));
                }
                else
                {
                    window.draw(linedraw(stateprint[0].getPosition().x + (desktop.width / 12.8), stateprint[0].getPosition().y + stateprint[0].getRadius(), stateprint[stateprint.size() - 1].getPosition().x + (desktop.width / 12.8), stateprint[stateprint.size() - 1].getPosition().y + stateprint[stateprint.size() - 1].getRadius()));
                    window.draw(linedraw(stateprint[0].getPosition().x + (desktop.width / 11.63), stateprint[0].getPosition().y + stateprint[0].getRadius(), stateprint[stateprint.size() - 1].getPosition().x + (desktop.width / 11.63), stateprint[stateprint.size() - 1].getPosition().y + stateprint[stateprint.size() - 1].getRadius()));
                }

                for (int i = 0; i < stateprint.size(); i++)
                {
                    window.draw(stateprint[i]);
                }

                for (int i = 0; i < finalsprint.size(); i++)
                {
                    window.draw(finalsprint[i]);
                }

                for (int i = 0; i < transitionprint.size(); i++)
                {
                    window.draw(transitionprint[i]);
                }

                for (int i = 0; i < statenameprint.size(); i++)
                {
                    window.draw(statenameprint[i]);
                }

                for (int i = 0; i < tprint.size(); i++)
                {
                    window.draw(tprint[i]);
                }
            }
            window.display();
        }
    }
}