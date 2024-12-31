#define NOMINMAX
#pragma warning(disable : 4996)
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include<SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <direct.h>
using namespace std;



struct FileNode
{
    std::string path;
    std::vector<FileNode*> children;
};
struct FileDetailsFirstPath
{
    std::string path;
    int numberChild;
    std::string name;
    int positionFather;
    bool director = false;
    int position;
    long long  size;
    bool print;
    char date;
} D[500];

struct FileDetailsSecondPath
{
    std::string path;
    int numberChild;
    std::string name;
    int positionFather;
    bool director = false;
    int position;
    long long  size;
    bool print;
    char date;
} C[500];

FileNode* createFileTree(const std::string& path)
{
    FileNode* node = new FileNode{ path };

    DIR* dir = opendir(path.c_str());
    if (dir != NULL)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            // ignora "." si ".."
            if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
            {
                continue;
            }
            std::string childPath = path + '/' + entry->d_name;
            struct stat statbuf;
            stat(childPath.c_str(), &statbuf);
            if (S_ISDIR(statbuf.st_mode))
            {
                // creaza recursiv vectorul de copii ai directorului
                node->children.push_back(createFileTree(childPath));
            }
            else
            {
                // creaza un nod frunza pentru fisier
                node->children.push_back(new FileNode{ childPath });
            }
        }
        closedir(dir);
    }

    return node;
}



std::string getNameFromPath(const std::string& path)
{
    size_t lastSlashPos = path.find_last_of('/');
    if (lastSlashPos == std::string::npos)
    {
        return path;
    }
    return path.substr(lastSlashPos + 1);
}

int numberChildren(FileNode* node)
{
    int count = 1;
    for (FileNode* child : node->children)
    {
        count += numberChildren(child);
    }
    return count;
}

void traverseDFS_FirstPath(FileNode* root, int& i, int p)
{

    i++;
    for (FileNode* child : root->children)
    {
        D[i].name = getNameFromPath(child->path);
        D[i].path = child->path;
        D[i].numberChild = numberChildren(child) - 1;
        D[i].positionFather = p;
        D[i].position = i;
        D[i].print = false;
        struct stat statDirector;
        stat(child->path.c_str(), &statDirector);
        if (S_ISDIR(statDirector.st_mode))
            D[i].director = true;
        struct stat statSize;
        stat(child->path.c_str(), &statSize);
        D[i].size = statSize.st_size;
        struct stat statDate;
        stat(child->path.c_str(), &statDate);
        D[i].date = statDate.st_mtime;
        traverseDFS_FirstPath(child, i, D[i].position);

    }
}

void traverseDFS_SecondPath(FileNode* root, int& i, int p)
{

    i++;
    for (FileNode* child : root->children)
    {
        C[i].name = getNameFromPath(child->path);
        C[i].path = child->path;
        C[i].numberChild = numberChildren(child) - 1;
        C[i].positionFather = p;
        C[i].position = i;
        C[i].print = false;
        struct stat statDirector;
        stat(child->path.c_str(), &statDirector);
        if (S_ISDIR(statDirector.st_mode))
            C[i].director = true;
        struct stat statSize;
        stat(child->path.c_str(), &statSize);
        C[i].size = statSize.st_size;
        struct stat statDate;
        stat(child->path.c_str(), &statDate);
        C[i].date = statDate.st_mtime;
        traverseDFS_SecondPath(child, i, C[i].position);

    }
}

void createFile(const std::string& path)
{
    const int length = path.length();
    char* char_array = new char[length + 1];
    strcpy(char_array, path.c_str());


    if (_mkdir(char_array) == -1)
        cout << "Nu s-a creat directorul.";
    else
        cout << "S-a creat directorul.";

}


void moveFile(const std::string& oldPath, const std::string& newPath)
{
    // Se deschide fisierul in directorul parinte
    std::ifstream in(oldPath, std::ios::binary);
    if (newPath.empty())
    {
        std::cerr << "Error opening input file: " << oldPath << '\n';
        return;
    }

    // Se deschide fisierul in directorul nou
    std::ofstream out(newPath, std::ios::binary);
    if (oldPath.empty())
    {
        std::cerr << "Error opening output file: " << newPath << '\n';
        return;
    }

    // Se copiaza informatia din directorul parinte in cel nou
    out << in.rdbuf();

    // Se inchid fisierele
    in.close();
    out.close();

    // Sterge fisierul de la adresa veche
    if (std::remove(oldPath.c_str()) != 0)
    {
        if (oldPath.empty())
            std::cerr << "Error deleting file: " << oldPath << '\n';
    }
}

void deleteFile(const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    if (path.empty())
    {
        std::cerr << "Error opening input file: " << path << '\n';
    }
    in.close();

    if (std::remove(path.c_str()) != 0)
    {
        if (path.empty())
            std::cerr << "Error deleting the file: " << path << '\n';
    }
}

void copyFile(std::string& oldPath, std::string& newPath)
{
    // Se deschide fisierul in directorul parinte
    std::ifstream in(oldPath, std::ios::binary);

    if (newPath.empty())
    {
        std::cerr << "Error opening input file: " << oldPath << '\n';
        return;

    }
    newPath = newPath + "/" + getNameFromPath(oldPath);

    // Se deschide fisierul in directorul nou
    std::ofstream out(newPath, std::ios::binary);
    if (oldPath.empty())
    {
        std::cerr << "Error opening output file: " << newPath << '\n';
        return;
    }

    // Se copiaza informatia din directorul parinte in cel nou
    out << in.rdbuf();

    // Se inchid fisierele
    in.close();
    out.close();
}

std::string getNameFromPath2(const std::string& path)
{
    size_t lastSlashPos = path.find_last_of('\\');
    if (lastSlashPos == std::string::npos)
    {
        return path;
    }
    return path.substr(lastSlashPos + 1);
}

void openFile(const std::string& filePath)
{
    std::string command = "start \"\" \"" + filePath + "\"";
    std::system(command.c_str());
}

void sortbyNameAscendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (D[v[i]].name > D[v[j]].name)
                swap(v[i], v[j]);

}

void sortbyNameDescendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (D[v[i]].name < D[v[j]].name)
                swap(v[i], v[j]);

}

void sortbyNameAscendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (C[v[i]].name > C[v[j]].name)
                swap(v[i], v[j]);

}

void sortbyNameDescendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (C[v[i]].name < C[v[j]].name)
                swap(v[i], v[j]);

}

void sortbySizeAscendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (D[v[i]].size > D[v[j]].size)
                swap(v[i], v[j]);

}

void sortbySizeDescendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (D[v[i]].size < D[v[j]].size)
                swap(v[i], v[j]);

}

void sortbySizeAscendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (C[v[i]].size > C[v[j]].size)
                swap(v[i], v[j]);

}

void sortbySizeDescendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (C[v[i]].size < C[v[j]].size)
                swap(v[i], v[j]);

}

void sortbyExtAscendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (D[v[i]].director > D[v[j]].director)
                swap(v[i], v[j]);

}

void sortbyExtDescendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (D[v[i]].director < D[v[j]].director)
                swap(v[i], v[j]);

}

void sortbyExtAscendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (C[v[i]].director > C[v[j]].director)
                swap(v[i], v[j]);

}

void sortbyExtDescendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
            if (C[v[i]].director < C[v[j]].director)
                swap(v[i], v[j]);

}


void sortbyDateAscendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
        {
            if (D[v[i]].date > D[v[j]].date)
                swap(v[i], v[j]);
        }

}

void sortbyDateDescendent1(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
        {
            if (D[v[i]].date < D[v[j]].date)
                swap(v[i], v[j]);
        }
}

void sortbyDateAscendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
        {
            if (C[v[i]].date > C[v[j]].date)
                swap(v[i], v[j]);
        }
}

void sortbyDateDescendent2(int v[], int n)
{
    for (int i = 1; i < n; i++)
        for (int j = i + 1; j <= n; j++)
        {
            if (C[v[i]].date < C[v[j]].date)
                swap(v[i], v[j]);
        }
}

int i, j, k, p, v1[300], v2[300], elemDraw[300], elemDraw2[300], filesSection1Part1, filesSection1Part2, filesSection2Part1, filesSection2Part2, cnt, pozFather, pozFather2, xSection1Part1 = 53, xSection1Part2 = 73,
totalFilesSection1, totalFilesSection2, pozSelectedFileSection1, pozSelectedFileSection2;
int  section1DateSort, section2DateSort, section1NameSort, section2NameSort, section1SizeSort, section2SizeSort,
section1ExtSort, section2ExtSort;
std::string lastClick;
std::string moveAt;
std::string pathDirector1;
std::string pathDirector2;
bool insideTextBox = false;
std::string inputString = "";
bool found = false;
bool F3_Clicked = false;
bool F4_Clicked = false;
bool F5_Clicked = false;
bool F6_Clicked = false;
bool F7_Clicked = false;
bool F8_Clicked = false;
bool primit = false;
bool  section1NameSortButton_Clicked = false;
bool  section2NameSortButton_Clicked = false;
bool  section1ExtSortButton_Clicked = false;
bool  section2ExtSortButton_Clicked = false;
bool  section1SizeSortButton_Clicked = false;
bool  section2SizeSortButton_Clicked = false;
bool  section1DateSortButton_Clicked = false;
bool  section2DateSortButton_Clicked = false;
int main()
{
    cout << "Introduceti doua adrese de directori";
    cin.get();
    getline(cin, pathDirector1);

    getline(cin, pathDirector2);


    sf::RenderWindow window(sf::VideoMode(1080, 600), "Total Commander", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        return 1;
    }
    sf::Image icon;
    icon.loadFromFile("icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::RectangleShape menuBar1(sf::Vector2f(1080, 20));
    menuBar1.setOutlineThickness(1.f);
    menuBar1.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape menuBar2(sf::Vector2f(1080, 51));
    menuBar2.setOutlineThickness(1.f);
    menuBar2.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape menuBar3(sf::Vector2f(1080, 25));
    menuBar3.setOutlineThickness(1.f);
    menuBar3.setPosition(0.f, 550.f);
    menuBar3.setOutlineColor(sf::Color(138, 138, 128));
    menuBar3.setFillColor(sf::Color(240, 240, 240));

    sf::RectangleShape textBox(sf::Vector2f(780, 20));
    textBox.setFillColor(sf::Color::White);
    textBox.setOutlineColor(sf::Color::Black);
    textBox.setOutlineThickness(1.f);
    textBox.setPosition(250, 552);

    sf::Text text("", font, 13);
    text.setFillColor(sf::Color::Black);
    text.setPosition(251, 553);


    sf::RectangleShape menuBar4(sf::Vector2f(1080, 25));
    menuBar4.setOutlineThickness(1.f);
    menuBar4.setOutlineColor(sf::Color(138, 138, 128));
    menuBar4.setPosition(0.f, 575.f);
    menuBar4.setFillColor(sf::Color(240, 240, 240));

    sf::RectangleShape menuBar6(sf::Vector2f(40, 497));
    menuBar6.setFillColor(sf::Color(240, 240, 240));
    menuBar6.setPosition(519.f, 52.f);
    menuBar6.setOutlineThickness(1.f);
    menuBar6.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape sizeBar1(sf::Vector2f(518, 20));
    sizeBar1.setFillColor(sf::Color(240, 240, 240));
    sizeBar1.setPosition(0.f, 529.f);
    sizeBar1.setOutlineThickness(1.f);
    sizeBar1.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape sizeBar2(sf::Vector2f(520, 20));
    sizeBar2.setFillColor(sf::Color(240, 240, 240));
    sizeBar2.setPosition(560.f, 529.f);
    sizeBar2.setOutlineThickness(1.f);
    sizeBar2.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape section1Part1(sf::Vector2f(199, 476));
    section1Part1.setPosition(0.f, 52.f);
    section1Part1.setOutlineThickness(1.f);
    section1Part1.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape verticalscrollBar1Background(sf::Vector2f(10, 465));
    verticalscrollBar1Background.setFillColor(sf::Color(240, 240, 240));
    verticalscrollBar1Background.setPosition(189.f, 52.f);
    verticalscrollBar1Background.setOutlineThickness(1.f);
    verticalscrollBar1Background.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape horizontalscrollBar1Background(sf::Vector2f(198, 10));
    horizontalscrollBar1Background.setFillColor(sf::Color(240, 240, 240));
    horizontalscrollBar1Background.setPosition(1.f, 518.f);
    horizontalscrollBar1Background.setOutlineThickness(1.f);
    horizontalscrollBar1Background.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape verticalscrollBar1(sf::Vector2f(10, 200));
    verticalscrollBar1.setFillColor(sf::Color(205, 205, 205));
    verticalscrollBar1.setPosition(189.f, 52.f);
    verticalscrollBar1.setOutlineThickness(1.f);
    verticalscrollBar1.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape horizontalscrollBar1(sf::Vector2f(70, 10));
    horizontalscrollBar1.setFillColor(sf::Color(205, 205, 205));
    horizontalscrollBar1.setPosition(1.f, 518.f);
    horizontalscrollBar1.setOutlineThickness(1.f);
    horizontalscrollBar1.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape verticalscrollBar2Background(sf::Vector2f(10, 465));
    verticalscrollBar2Background.setFillColor(sf::Color(240, 240, 240));
    verticalscrollBar2Background.setPosition(750.f, 52.f);
    verticalscrollBar2Background.setOutlineThickness(1.f);
    verticalscrollBar2Background.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape horizontalscrollBar2Background(sf::Vector2f(198, 10));
    horizontalscrollBar2Background.setFillColor(sf::Color(240, 240, 240));
    horizontalscrollBar2Background.setPosition(562.f, 518.f);
    horizontalscrollBar2Background.setOutlineThickness(1.f);
    horizontalscrollBar2Background.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape verticalscrollBar2(sf::Vector2f(10, 200));
    verticalscrollBar2.setFillColor(sf::Color(205, 205, 205));
    verticalscrollBar2.setPosition(750.f, 52.f);
    verticalscrollBar2.setOutlineThickness(1.f);
    verticalscrollBar2.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape horizontalscrollBar2(sf::Vector2f(70, 10));
    horizontalscrollBar2.setFillColor(sf::Color(205, 205, 205));
    horizontalscrollBar2.setPosition(562.f, 518.f);
    horizontalscrollBar2.setOutlineThickness(1.f);
    horizontalscrollBar2.setOutlineColor(sf::Color(138, 138, 128));


    sf::RectangleShape section1Part2(sf::Vector2f(319, 456));
    section1Part2.setPosition(201.f, 72.f);
    section1Part2.setOutlineThickness(1.f);
    section1Part2.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape section2Part1(sf::Vector2f(199, 476));
    section2Part1.setPosition(561.f, 52.f);
    section2Part1.setOutlineThickness(1.f);
    section2Part1.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape section2Part2(sf::Vector2f(319, 456));
    section2Part2.setPosition(762.f, 72.f);
    section2Part2.setOutlineThickness(1.f);
    section2Part2.setOutlineColor(sf::Color(138, 138, 128));

    sf::RectangleShape section1NameSortButton(sf::Vector2f(78, 19));
    section1NameSortButton.setPosition(201.f, 52.f);
    section1NameSortButton.setFillColor(sf::Color::White);
    section1NameSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section1NameSortButton.setOutlineThickness(1.f);

    sf::Text section1NameSortButtonText;
    section1NameSortButtonText.setFont(font);
    section1NameSortButtonText.setString("Nume");
    section1NameSortButtonText.setCharacterSize(12);
    section1NameSortButtonText.setFillColor(sf::Color::Black);
    section1NameSortButtonText.setPosition(220, 54);

    sf::RectangleShape section1ExtSortButton(sf::Vector2f(78, 19));
    section1ExtSortButton.setPosition(279.f, 52.f);
    section1ExtSortButton.setFillColor(sf::Color::White);
    section1ExtSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section1ExtSortButton.setOutlineThickness(1.f);

    sf::Text section1ExtSortButtonText;
    section1ExtSortButtonText.setFont(font);
    section1ExtSortButtonText.setString("Ext.");
    section1ExtSortButtonText.setCharacterSize(12);
    section1ExtSortButtonText.setFillColor(sf::Color::Black);
    section1ExtSortButtonText.setPosition(299, 54);

    sf::RectangleShape section1SizeSortButton(sf::Vector2f(78, 19));
    section1SizeSortButton.setPosition(357.f, 52.f);
    section1SizeSortButton.setFillColor(sf::Color::White);
    section1SizeSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section1SizeSortButton.setOutlineThickness(1.f);

    sf::Text section1SizeSortButtonText;
    section1SizeSortButtonText.setFont(font);
    section1SizeSortButtonText.setString("Marime");
    section1SizeSortButtonText.setCharacterSize(12);
    section1SizeSortButtonText.setFillColor(sf::Color::Black);
    section1SizeSortButtonText.setPosition(377, 54);

    sf::RectangleShape section1DateSortButton(sf::Vector2f(82, 19));
    section1DateSortButton.setPosition(435.f, 52.f);
    section1DateSortButton.setFillColor(sf::Color::White);
    section1DateSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section1DateSortButton.setOutlineThickness(1.f);

    sf::Text section1DateSortButtonText;
    section1DateSortButtonText.setFont(font);
    section1DateSortButtonText.setString("Data");
    section1DateSortButtonText.setCharacterSize(12);
    section1DateSortButtonText.setFillColor(sf::Color::Black);
    section1DateSortButtonText.setPosition(455, 54);

    sf::RectangleShape section2NameSortButton(sf::Vector2f(78, 19));
    section2NameSortButton.setPosition(762.f, 52.f);
    section2NameSortButton.setFillColor(sf::Color::White);
    section2NameSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section2NameSortButton.setOutlineThickness(1.f);

    sf::Text section2NameSortButtonText;
    section2NameSortButtonText.setFont(font);
    section2NameSortButtonText.setString("Nume");
    section2NameSortButtonText.setCharacterSize(12);
    section2NameSortButtonText.setFillColor(sf::Color::Black);
    section2NameSortButtonText.setPosition(781, 54);

    sf::RectangleShape section2ExtSortButton(sf::Vector2f(78, 19));
    section2ExtSortButton.setPosition(840.f, 52.f);
    section2ExtSortButton.setFillColor(sf::Color::White);
    section2ExtSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section2ExtSortButton.setOutlineThickness(1.f);

    sf::Text section2ExtSortButtonText;
    section2ExtSortButtonText.setFont(font);
    section2ExtSortButtonText.setString("Ext.");
    section2ExtSortButtonText.setCharacterSize(12);
    section2ExtSortButtonText.setFillColor(sf::Color::Black);
    section2ExtSortButtonText.setPosition(859, 54);

    sf::RectangleShape section2SizeSortButton(sf::Vector2f(78, 19));
    section2SizeSortButton.setPosition(918.f, 52.f);
    section2SizeSortButton.setFillColor(sf::Color::White);
    section2SizeSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section2SizeSortButton.setOutlineThickness(1.f);

    sf::Text section2SizeSortButtonText;
    section2SizeSortButtonText.setFont(font);
    section2SizeSortButtonText.setString("Marime");
    section2SizeSortButtonText.setCharacterSize(12);
    section2SizeSortButtonText.setFillColor(sf::Color::Black);
    section2SizeSortButtonText.setPosition(937, 54);

    sf::RectangleShape section2DateSortButton(sf::Vector2f(84, 19));
    section2DateSortButton.setPosition(996.f, 52.f);
    section2DateSortButton.setFillColor(sf::Color::White);
    section2DateSortButton.setOutlineColor(sf::Color(138, 138, 128));
    section2DateSortButton.setOutlineThickness(1.f);

    sf::Text section2DateSortButtonText;
    section2DateSortButtonText.setFont(font);
    section2DateSortButtonText.setString("Data");
    section2DateSortButtonText.setCharacterSize(12);
    section2DateSortButtonText.setFillColor(sf::Color::Black);
    section2DateSortButtonText.setPosition(1015, 54);

    sf::RectangleShape F3_Button(sf::Vector2f(154, 23));
    F3_Button.setPosition(0.f, 576.f);
    F3_Button.setFillColor(sf::Color(240, 240, 240));
    F3_Button.setOutlineColor(sf::Color(138, 138, 128));
    F3_Button.setOutlineThickness(1.f);

    sf::Text F3_ButtonText;
    F3_ButtonText.setFont(font);
    F3_ButtonText.setString("F3 Vizualizare");
    F3_ButtonText.setCharacterSize(12);
    F3_ButtonText.setFillColor(sf::Color::Black);
    F3_ButtonText.setPosition(40, 581);

    sf::RectangleShape F4_Button(sf::Vector2f(153, 23));
    F4_Button.setPosition(155.f, 576.f);
    F4_Button.setFillColor(sf::Color(240, 240, 240));
    F4_Button.setOutlineColor(sf::Color(138, 138, 128));
    F4_Button.setOutlineThickness(1.f);

    sf::Text F4_ButtonText;
    F4_ButtonText.setFont(font);
    F4_ButtonText.setString("F4 Editare");
    F4_ButtonText.setCharacterSize(12);
    F4_ButtonText.setFillColor(sf::Color::Black);
    F4_ButtonText.setPosition(194, 581);

    sf::RectangleShape F5_Button(sf::Vector2f(153, 23));
    F5_Button.setPosition(309.f, 576.f);
    F5_Button.setFillColor(sf::Color(240, 240, 240));
    F5_Button.setOutlineColor(sf::Color(138, 138, 128));
    F5_Button.setOutlineThickness(1.f);

    sf::Text F5_ButtonText;
    F5_ButtonText.setFont(font);
    F5_ButtonText.setString("F5 Copiere");
    F5_ButtonText.setCharacterSize(12);
    F5_ButtonText.setFillColor(sf::Color::Black);
    F5_ButtonText.setPosition(347, 581);

    sf::RectangleShape F6_Button(sf::Vector2f(153, 23));
    F6_Button.setPosition(463.f, 576.f);
    F6_Button.setFillColor(sf::Color(240, 240, 240));
    F6_Button.setOutlineColor(sf::Color(138, 138, 128));
    F6_Button.setOutlineThickness(1.f);

    sf::Text F6_ButtonText;
    F6_ButtonText.setFont(font);
    F6_ButtonText.setString("F6 Mutare");
    F6_ButtonText.setCharacterSize(12);
    F6_ButtonText.setFillColor(sf::Color::Black);
    F6_ButtonText.setPosition(500, 581);

    sf::RectangleShape F7_Button(sf::Vector2f(153, 23));
    F7_Button.setPosition(617.f, 576.f);
    F7_Button.setFillColor(sf::Color(240, 240, 240));
    F7_Button.setOutlineColor(sf::Color(138, 138, 128));
    F7_Button.setOutlineThickness(1.f);

    sf::Text F7_ButtonText;
    F7_ButtonText.setFont(font);
    F7_ButtonText.setString("F7 Folder Nou");
    F7_ButtonText.setCharacterSize(12);
    F7_ButtonText.setFillColor(sf::Color::Black);
    F7_ButtonText.setPosition(653, 581);

    sf::RectangleShape F8_Button(sf::Vector2f(153, 23));
    F8_Button.setPosition(771.f, 576.f);
    F8_Button.setFillColor(sf::Color(240, 240, 240));
    F8_Button.setOutlineColor(sf::Color(138, 138, 128));
    F8_Button.setOutlineThickness(1.f);

    sf::Text F8_ButtonText;
    F8_ButtonText.setFont(font);
    F8_ButtonText.setString("F8 Stergere");
    F8_ButtonText.setCharacterSize(12);
    F8_ButtonText.setFillColor(sf::Color::Black);
    F8_ButtonText.setPosition(806, 581);

    sf::RectangleShape Alt_F4_Button(sf::Vector2f(154, 23));
    Alt_F4_Button.setPosition(925.f, 576.f);
    Alt_F4_Button.setFillColor(sf::Color(240, 240, 240));
    Alt_F4_Button.setOutlineColor(sf::Color(138, 138, 128));
    Alt_F4_Button.setOutlineThickness(1.f);

    sf::Text Alt_F4_ButtonText;
    Alt_F4_ButtonText.setFont(font);
    Alt_F4_ButtonText.setString("Alt+F4:Iesire");
    Alt_F4_ButtonText.setCharacterSize(12);
    Alt_F4_ButtonText.setFillColor(sf::Color::Black);
    Alt_F4_ButtonText.setPosition(959, 581);


    const int numButtons = 8;
    sf::Text buttons[numButtons];
    sf::Color buttonColors[numButtons] = { sf::Color::Black };
    std::string buttonText[numButtons] = { "Fisiere", "Selectii", "Comenzi",
                                           "Retea","Afisare","Configurare", "Start", "Ajutor"
    };

    for (k = 0; k < numButtons; k++)
    {
        buttons[k].setString(buttonText[k]);
        buttons[k].setFont(font);
        buttons[k].setCharacterSize(11);
        buttons[k].setFillColor(buttonColors[k]);
        buttons[k].setPosition(10 + k * 60, 5);
    }
    buttons[6].setPosition(400, 5);
    buttons[7].setPosition(1020, 5);

    window.setKeyRepeatEnabled(false);

    sf::Texture icons1;
    sf::Sprite Icons1;
    if (!icons1.loadFromFile("Icons1.png"))
        std::cout << "Erorr" << std::endl;
    Icons1.setTexture(icons1);
    Icons1.setPosition(5, 25);

    sf::Texture icons2;
    sf::Sprite Icons2;
    if (!icons2.loadFromFile("Icons2.png"))
        std::cout << "Erorr" << std::endl;
    Icons2.setTexture(icons2);
    Icons2.setPosition(525, 65);

    sf::Text Section1Part1Button[500];
    sf::Text Section1Part2Button[500];
    FileNode* root = createFileTree(pathDirector1);
    traverseDFS_FirstPath(root, totalFilesSection1, p);
    D[0].director = true;
    D[0].position = 0;
    D[0].print = true;
    v1[0] = 0;
    D[0].positionFather = -1;
    Section1Part1Button[0].setString(getNameFromPath2(pathDirector1));
    Section1Part1Button[0].setCharacterSize(13);
    Section1Part1Button[0].setFillColor(sf::Color::Black);
    Section1Part1Button[0].setFont(font);
    Section1Part1Button[0].setPosition(40, xSection1Part1 + 15);


    sf::Text Section2Part1Button[500];
    sf::Text Section2Part2Button[500];
    FileNode* root1 = createFileTree(pathDirector2);
    traverseDFS_SecondPath(root1, totalFilesSection2, p);
    C[0].director = true;
    C[0].position = 0;
    C[0].print = true;
    v2[0] = 0;
    C[0].positionFather = -1;
    Section2Part1Button[0].setString(getNameFromPath2(pathDirector2));
    Section2Part1Button[0].setCharacterSize(13);
    Section2Part1Button[0].setFillColor(sf::Color::Black);
    Section2Part1Button[0].setFont(font);
    Section2Part1Button[0].setPosition(592, xSection1Part1 + 15);



    sf::Texture iconDirector;
    sf::Sprite IconDirector;
    if (!iconDirector.loadFromFile("IconDirector.png"))
        std::cout << "Erorr" << std::endl;
    IconDirector.setTexture(iconDirector);
    IconDirector.setPosition(30, xSection1Part1);
    IconDirector.setScale(0.8, 0.8);

    sf::Texture iconFisier;
    sf::Sprite IconFisier;
    if (!iconFisier.loadFromFile("IconFisier.png"))
        std::cout << "Erorr" << std::endl;
    IconFisier.setTexture(iconFisier);
    IconFisier.setScale(0.8, 0.8);


    while (window.isOpen())
    {

        text.setString(inputString);
        for (int j = 1; j <= totalFilesSection1; j++)
        {


            Section1Part1Button[j].setString(D[j].name);
            Section1Part1Button[j].setCharacterSize(13);
            Section1Part1Button[j].setFillColor(sf::Color::Black);
            Section1Part1Button[j].setFont(font);

            Section1Part2Button[j].setString(D[j].name);
            Section1Part2Button[j].setCharacterSize(13);
            Section1Part2Button[j].setFillColor(sf::Color::Black);
            Section1Part2Button[j].setFont(font);
        }
        for (int j = 1; j <= totalFilesSection2; j++)
        {


            Section2Part1Button[j].setString(C[j].name);
            Section2Part1Button[j].setCharacterSize(13);
            Section2Part1Button[j].setFillColor(sf::Color::Black);
            Section2Part1Button[j].setFont(font);

            Section2Part2Button[j].setString(C[j].name);
            Section2Part2Button[j].setCharacterSize(13);
            Section2Part2Button[j].setFillColor(sf::Color::Black);
            Section2Part2Button[j].setFont(font);
        }
        sf::Event event;
        while (window.pollEvent(event))
        {


            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
            if (event.type == sf::Event::MouseMoved)
            {
                sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                insideTextBox = textBox.getGlobalBounds().contains(mousePos);
            }


            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (insideTextBox)
                {
                    inputString = "";
                }
            }


            if (event.type == sf::Event::TextEntered && insideTextBox)
            {
                if (event.text.unicode == 8 && inputString.length() > 0)
                {

                    inputString.pop_back();
                }
                else
                    inputString += event.text.unicode;
            }

        }
        text.setString(inputString);
        sf::Vector2f mousePos1 = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (k = 0; k < numButtons; k++)
        {
            if (buttons[k].getGlobalBounds().contains(mousePos1))
            {
                buttons[k].setFillColor(sf::Color(216, 230, 242));
            }
            else
            {
                buttons[k].setFillColor(buttonColors[k]);
            }
        }

        if (F3_Button.getGlobalBounds().contains(mousePos1))
        {
            F3_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            F3_Button.setFillColor(sf::Color(240, 240, 240));
        }

        if (F4_Button.getGlobalBounds().contains(mousePos1))
        {
            F4_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            F4_Button.setFillColor(sf::Color(240, 240, 240));
        }
        if (F5_Button.getGlobalBounds().contains(mousePos1))
        {
            F5_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            F5_Button.setFillColor(sf::Color(240, 240, 240));
        }
        if (F6_Button.getGlobalBounds().contains(mousePos1))
        {
            F6_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            F6_Button.setFillColor(sf::Color(240, 240, 240));
        }
        if (F7_Button.getGlobalBounds().contains(mousePos1))
        {
            F7_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            F7_Button.setFillColor(sf::Color(240, 240, 240));
        }
        if (F8_Button.getGlobalBounds().contains(mousePos1))
        {
            F8_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            F8_Button.setFillColor(sf::Color(240, 240, 240));
        }

        if (Alt_F4_Button.getGlobalBounds().contains(mousePos1))
        {
            Alt_F4_Button.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            Alt_F4_Button.setFillColor(sf::Color(240, 240, 240));
        }

        if (section1NameSortButton.getGlobalBounds().contains(mousePos1))
        {
            section1NameSortButton.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            section1NameSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section2NameSortButton.getGlobalBounds().contains(mousePos1))
        {
            section2NameSortButton.setFillColor(sf::Color::White);
        }
        else
        {
            section2NameSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section1ExtSortButton.getGlobalBounds().contains(mousePos1))
        {
            section1ExtSortButton.setFillColor(sf::Color::White);
        }
        else
        {
            section1ExtSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section2ExtSortButton.getGlobalBounds().contains(mousePos1))
        {
            section2ExtSortButton.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            section2ExtSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section1SizeSortButton.getGlobalBounds().contains(mousePos1))
        {
            section1SizeSortButton.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            section1SizeSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section2SizeSortButton.getGlobalBounds().contains(mousePos1))
        {
            section2SizeSortButton.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            section2SizeSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section1DateSortButton.getGlobalBounds().contains(mousePos1))
        {
            section1DateSortButton.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            section1DateSortButton.setFillColor(sf::Color(240, 240, 240));
        }

        if (section2DateSortButton.getGlobalBounds().contains(mousePos1))
        {
            section2DateSortButton.setFillColor(sf::Color(216, 230, 242));
        }
        else
        {
            section2DateSortButton.setFillColor(sf::Color(240, 240, 240));
        }



        for (k = 0; k <= totalFilesSection1; k++)
        {
            if (Section1Part1Button[k].getGlobalBounds().contains(mousePos1))
            {
                Section1Part1Button[k].setFillColor(sf::Color(216, 230, 242));
            }

            else

                Section1Part1Button[k].setFillColor(sf::Color::Black);

        }

        for (k = 0; k <= totalFilesSection1; k++)
        {
            if (Section1Part2Button[k].getGlobalBounds().contains(mousePos1))
            {
                Section1Part2Button[k].setFillColor(sf::Color(216, 230, 242));
            }
            else

                Section1Part2Button[k].setFillColor(sf::Color::Black);

        }
        for (k = 0; k <= totalFilesSection2; k++)
        {
            if (Section2Part1Button[k].getGlobalBounds().contains(mousePos1))
            {
                Section2Part1Button[k].setFillColor(sf::Color(216, 230, 242));
            }
            else

                Section2Part1Button[k].setFillColor(sf::Color::Black);

        }
        for (k = 0; k <= totalFilesSection2; k++)
        {
            if (Section2Part2Button[k].getGlobalBounds().contains(mousePos1))
            {
                Section2Part2Button[k].setFillColor(sf::Color(216, 230, 242));
            }
            else

                Section2Part2Button[k].setFillColor(sf::Color::Black);

        }

        if (event.type == sf::Event::MouseMoved)
        {
            sf::Vector2i mousePos2 = sf::Mouse::getPosition(window);
            if (horizontalscrollBar1.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos2)))
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    if (event.mouseMove.x - 40 > 1 && event.mouseMove.x - 40 < 139)
                        horizontalscrollBar1.setPosition(event.mouseMove.x - 40, horizontalscrollBar1.getPosition().y);
                }
            }

            if (verticalscrollBar1.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos2)))
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    if (event.mouseMove.y - 20 > 52 && event.mouseMove.y - 20 < 328)
                        verticalscrollBar1.setPosition(verticalscrollBar1.getPosition().x, event.mouseMove.y - 20);
                }
            }

            if (horizontalscrollBar2.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos2)))
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    if (event.mouseMove.x - 40 > 540 && event.mouseMove.x - 40 < 690)
                        horizontalscrollBar2.setPosition(event.mouseMove.x - 40, horizontalscrollBar2.getPosition().y);
                }
            }

            if (verticalscrollBar2.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos2)))
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    if (event.mouseMove.y - 20 > 52 && event.mouseMove.y - 20 < 328)
                        verticalscrollBar2.setPosition(verticalscrollBar2.getPosition().x, event.mouseMove.y - 20);
                }
            }

        }
        found = false;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {


            xSection1Part1 = 53;
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            for (j = 0; j <= totalFilesSection1 && !found; j++)
                if (Section1Part1Button[j].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    Section1Part1Button[pozFather].setFillColor(sf::Color::Black);
                    moveAt = D[j].path;
                    pozFather = D[j].position;
                    found = true;
                }
            cnt = 1;

            for (k = 0; k <= totalFilesSection1; k++)
                if (D[k].positionFather == pozFather || D[k].print == true)
                {
                    if (D[k].print && D[k].director)
                    {
                        Section1Part1Button[k].setPosition(40 + v1[k] * 20, xSection1Part1 + cnt * 15);
                        cnt++;
                    }

                    else
                    {


                        D[k].print = true;
                        if (D[k].director)
                        {
                            v1[k] = v1[pozFather] + 1;
                            Section1Part1Button[k].setPosition(40 + v1[k] * 20, xSection1Part1 + cnt * 15);
                            cnt++;

                        }
                    }



                }

            for (k = 0; k <= totalFilesSection1 && !found; k++)
                if (Section1Part2Button[k].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    Section1Part2Button[k].setFillColor(sf::Color::Red);

                    pozSelectedFileSection1 = k;
                    lastClick = D[k].path;
                    found = true;
                }

            xSection1Part1 = 53;

            for (i = 0; i <= totalFilesSection2 && !found; i++)
                if (Section2Part1Button[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    Section2Part1Button[pozFather2].setFillColor(sf::Color::Black);
                    moveAt = C[i].path;
                    pozFather2 = C[i].position;
                    found = true;
                }
            cnt = 1;

            for (k = 0; k <= totalFilesSection2; k++)
                if (C[k].positionFather == pozFather2 || C[k].print == true)
                {
                    if (C[k].print && C[k].director)
                    {
                        Section2Part1Button[k].setPosition(592 + v2[k] * 20, xSection1Part1 + cnt * 15);
                        cnt++;
                    }

                    else
                    {


                        C[k].print = true;
                        if (C[k].director)
                        {
                            v2[k] = v2[pozFather2] + 1;
                            Section2Part1Button[k].setPosition(592 + v2[k] * 20, xSection1Part1 + cnt * 15);
                            cnt++;

                        }
                    }



                }

            for (k = 0; k <= totalFilesSection2 && !found; k++)
                if (Section2Part2Button[k].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    pozSelectedFileSection2 = k;
                    lastClick = C[k].path;
                    found = true;
                }

            if (F3_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                F3_Clicked = true;
            }

            if (F4_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                F4_Clicked = true;
            }

            if (F5_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) && F5_Clicked == false)
            {
                F5_Clicked = true;
            }

            if (F6_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                F6_Clicked = true;
            }

            if (F7_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                F7_Clicked = true;
            }

            if (F8_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                F8_Clicked = true;
            }

            if (Alt_F4_Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                window.close();
            }

            if (section1DateSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section1DateSortButton_Clicked = true;
                if (section1DateSort == 0)
                    section1DateSort = 1;
                else if (section1DateSort == 1)
                    section1DateSort = 2;
                else if (section1DateSort == 2)
                    section1DateSort = 1;
                section1ExtSort = 0;
                section1SizeSort = 0;
                section1NameSort = 0;
                section1ExtSortButton_Clicked = false;
                section1SizeSortButton_Clicked = false;
                section1NameSortButton_Clicked = false;
            }

            if (section2DateSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section2DateSortButton_Clicked = true;
                if (section2DateSort == 0)
                    section2DateSort = 1;
                else if (section2DateSort == 1)
                    section2DateSort = 2;
                else if (section2DateSort == 2)
                    section2DateSort = 1;
                section2ExtSort = 0;
                section2SizeSort = 0;
                section2NameSort = 0;
                section2ExtSortButton_Clicked = false;
                section2SizeSortButton_Clicked = false;
                section2NameSortButton_Clicked = false;

            }

            if (section1NameSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section1NameSortButton_Clicked = true;
                if (section1NameSort == 0)
                    section1NameSort = 1;
                else if (section1NameSort == 1)
                    section1NameSort = 2;
                else if (section1NameSort == 2)
                    section1NameSort = 1;
                section1ExtSort = 0;
                section1SizeSort = 0;
                section1DateSort = 0;
                section1ExtSortButton_Clicked = false;
                section1SizeSortButton_Clicked = false;
                section1DateSortButton_Clicked = false;
            }

            if (section2NameSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section2NameSortButton_Clicked = true;
                if (section2NameSort == 0)
                    section2NameSort = 1;
                else if (section2NameSort == 1)
                    section2NameSort = 2;
                else if (section2NameSort == 2)
                    section2NameSort = 1;
                section2ExtSort = 0;
                section2SizeSort = 0;
                section2DateSort = 0;
                section2ExtSortButton_Clicked = false;
                section2SizeSortButton_Clicked = false;
                section2DateSortButton_Clicked = false;

            }

            if (section1SizeSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section1SizeSortButton_Clicked = true;
                if (section1SizeSort == 0)
                    section1SizeSort = 1;
                else if (section1SizeSort == 1)
                    section1SizeSort = 2;
                else if (section1SizeSort == 2)
                    section1SizeSort = 1;
                section1ExtSort = 0;
                section1NameSort = 0;
                section1DateSort = 0;
                section1ExtSortButton_Clicked = false;
                section1DateSortButton_Clicked = false;
                section1NameSortButton_Clicked = false;

            }

            if (section2SizeSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section2SizeSortButton_Clicked = true;
                if (section2SizeSort == 0)
                    section2SizeSort = 1;
                else if (section2SizeSort == 1)
                    section2SizeSort = 2;
                else if (section2SizeSort == 2)
                    section2SizeSort = 1;
                section2ExtSort = 0;
                section2NameSort = 0;
                section2DateSort = 0;
                section2ExtSortButton_Clicked = false;
                section2DateSortButton_Clicked = false;
                section2NameSortButton_Clicked = false;
            }

            if (section1ExtSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section1ExtSortButton_Clicked = true;
                if (section1ExtSort == 0)
                    section1ExtSort = 1;
                else if (section1ExtSort == 1)
                    section1ExtSort = 2;
                else if (section1ExtSort == 2)
                    section1ExtSort = 1;
                section1SizeSort = 0;
                section1NameSort = 0;
                section1DateSort = 0;
                section1DateSortButton_Clicked = false;
                section1SizeSortButton_Clicked = false;
                section1NameSortButton_Clicked = false;
            }

            if (section2ExtSortButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                section2ExtSortButton_Clicked = true;
                if (section2ExtSort == 0)
                    section2ExtSort = 1;
                else if (section2ExtSort == 1)
                    section2ExtSort = 2;
                else if (section2ExtSort == 2)
                    section2ExtSort = 1;
                section2SizeSort = 0;
                section2NameSort = 0;
                section2DateSort = 0;
                section2DateSortButton_Clicked = false;
                section2SizeSortButton_Clicked = false;
                section2NameSortButton_Clicked = false;
            }
        }

        window.clear(sf::Color::White);
        window.setFramerateLimit(7);
        window.draw(menuBar4);
        window.draw(menuBar3);
        window.draw(menuBar2);
        window.draw(Icons1);
        window.draw(menuBar1);
        window.draw(sizeBar1);
        window.draw(sizeBar2);
        window.draw(section1Part1);
        cnt = 0;
        for (int k = 0; k <= totalFilesSection1; k++)
        {
            Section1Part2Button[k].setPosition(3000, 2000);
        }

        filesSection1Part1 = 0;
        filesSection1Part2 = 0;
        Section1Part1Button[j - 1].setFillColor(sf::Color::Red);
        Section1Part2Button[pozSelectedFileSection1].setFillColor(sf::Color::Red);

        for (int k = 0; k <= totalFilesSection1; k++)
        {
            if (k == 0)
            {
                filesSection1Part1++;
                window.draw(Section1Part1Button[k]);
                IconDirector.setPosition(20 + v1[k] * 20, xSection1Part1 + filesSection1Part1 * 15);
                window.draw(IconDirector);
            }
            else if (D[k].print == true)
            {
                if (D[k].director)
                {
                    filesSection1Part1++;
                    window.draw(Section1Part1Button[k]);
                    IconDirector.setPosition(20 + v1[k] * 20, xSection1Part1 + filesSection1Part1 * 15);
                    window.draw(IconDirector);
                }


            }
            if (D[k].positionFather == pozFather)
                elemDraw2[++cnt] = k;
        }

        window.draw(section1Part2);

        if (section1NameSortButton_Clicked)
        {
            if (section1NameSort == 1)
                sortbyNameAscendent1(elemDraw2, cnt);
            else if (section1NameSort == 2)
                sortbyNameDescendent1(elemDraw2, cnt);

        }
        if (section1DateSortButton_Clicked)
        {
            if (section1DateSort == 1)
                sortbyDateAscendent1(elemDraw2, cnt);
            else if (section1DateSort == 2)
                sortbyDateDescendent1(elemDraw2, cnt);

        }
        if (section1ExtSortButton_Clicked)
        {
            if (section1ExtSort == 1)
                sortbyExtAscendent1(elemDraw2, cnt);
            else if (section1ExtSort == 2)
                sortbyExtDescendent1(elemDraw2, cnt);

        }
        if (section1SizeSortButton_Clicked)
        {
            if (section1SizeSort == 1)
                sortbySizeAscendent1(elemDraw2, cnt);
            else if (section1SizeSort == 2)
                sortbySizeDescendent1(elemDraw2, cnt);

        }
        for (k = 1; k <= cnt; k++)
            if (D[elemDraw2[k]].director)
            {

                filesSection1Part2++;
                if (xSection1Part2 + filesSection1Part2 * 15 < 508)
                {
                    Section1Part2Button[elemDraw2[k]].setPosition(233, xSection1Part2 + filesSection1Part2 * 15);
                    window.draw(Section1Part2Button[elemDraw2[k]]);
                    IconDirector.setPosition(213, xSection1Part2 + filesSection1Part2 * 15);
                    window.draw(IconDirector);
                }
            }
            else
            {
                if (pozFather != 0)
                {
                    filesSection1Part2++;
                    if (xSection1Part2 + filesSection1Part2 * 15 < 508)
                    {
                        Section1Part2Button[elemDraw2[k]].setPosition(233, xSection1Part2 + filesSection1Part2 * 15);
                        window.draw(Section1Part2Button[elemDraw2[k]]);
                        IconFisier.setPosition(213, xSection1Part2 + filesSection1Part2 * 15);
                        window.draw(IconFisier);
                    }
                }
            }
        for (k = 0; k <= totalFilesSection2; k++)
        {
            Section2Part2Button[k].setPosition(3000, 2000);
        }
        filesSection2Part1 = 0;
        filesSection2Part2 = 0;
        cnt = 0;
        Section2Part1Button[i - 1].setFillColor(sf::Color::Red);
        Section2Part2Button[pozSelectedFileSection2].setFillColor(sf::Color::Red);
        window.draw(section2Part1);
        for (int k = 0; k <= totalFilesSection2; k++)
        {
            if (k == 0)
            {
                filesSection2Part1++;
                window.draw(Section2Part1Button[k]);
                IconDirector.setPosition(572 + v2[k] * 20, xSection1Part1 + filesSection2Part1 * 15);
                window.draw(IconDirector);
            }
            else if (C[k].print == true)
            {
                if (C[k].director)
                {
                    filesSection2Part1++;
                    window.draw(Section2Part1Button[k]);
                    IconDirector.setPosition(572 + v2[k] * 20, xSection1Part1 + filesSection2Part1 * 15);
                    window.draw(IconDirector);
                }


            }
            if (C[k].positionFather == pozFather2)
                elemDraw[++cnt] = k;

        }

        window.draw(section2Part2);
        if (section2NameSortButton_Clicked)
        {
            if (section2NameSort == 1)
                sortbyNameAscendent2(elemDraw, cnt);
            else if (section2NameSort == 2)
                sortbyNameDescendent2(elemDraw, cnt);

        }
        if (section2DateSortButton_Clicked)
        {
            if (section2DateSort == 1)
                sortbyDateAscendent2(elemDraw, cnt);
            else if (section2DateSort == 2)
                sortbyDateDescendent2(elemDraw, cnt);

        }
        if (section2ExtSortButton_Clicked)
        {
            if (section2ExtSort == 1)
                sortbyExtAscendent2(elemDraw, cnt);
            else if (section2ExtSort == 2)
                sortbyExtDescendent2(elemDraw, cnt);

        }
        if (section2SizeSortButton_Clicked)
        {
            if (section2SizeSort == 1)
                sortbySizeAscendent2(elemDraw, cnt);
            else if (section2SizeSort == 2)
                sortbySizeDescendent2(elemDraw, cnt);

        }
        for (k = 1; k <= cnt; k++)
            if (C[elemDraw[k]].director)
            {

                filesSection2Part2++;
                if (xSection1Part2 + filesSection2Part2 * 15 < 508)
                {
                    Section2Part2Button[elemDraw[k]].setPosition(800, xSection1Part2 + filesSection2Part2 * 15);
                    window.draw(Section2Part2Button[elemDraw[k]]);
                    IconDirector.setPosition(780, xSection1Part2 + filesSection2Part2 * 15);
                    window.draw(IconDirector);
                }
            }
            else
            {
                if (pozFather2 != 0)
                {
                    filesSection2Part2++;
                    if (xSection1Part2 + filesSection2Part2 * 15 < 508)
                    {
                        Section2Part2Button[elemDraw[k]].setPosition(800, xSection1Part2 + filesSection2Part2 * 15);
                        window.draw(Section2Part2Button[elemDraw[k]]);
                        IconFisier.setPosition(780, xSection1Part2 + filesSection2Part2 * 15);
                        window.draw(IconFisier);
                    }
                }
            }

        window.draw(F3_Button);
        window.draw(F3_ButtonText);
        window.draw(F4_Button);
        window.draw(F4_ButtonText);
        window.draw(F5_Button);
        window.draw(F5_ButtonText);
        window.draw(F6_Button);
        window.draw(F6_ButtonText);
        window.draw(F7_Button);
        window.draw(F7_ButtonText);
        window.draw(F8_Button);
        window.draw(F8_ButtonText);
        window.draw(section1NameSortButton);
        window.draw(section1NameSortButtonText);
        window.draw(section1ExtSortButton);
        window.draw(section1ExtSortButtonText);
        window.draw(section1SizeSortButton);
        window.draw(section1SizeSortButtonText);
        window.draw(section1DateSortButton);
        window.draw(section1DateSortButtonText);
        window.draw(section2NameSortButton);
        window.draw(section2NameSortButtonText);
        window.draw(section2ExtSortButton);
        window.draw(section2ExtSortButtonText);
        window.draw(section2SizeSortButton);
        window.draw(section2SizeSortButtonText);
        window.draw(section2DateSortButton);
        window.draw(section2DateSortButtonText);
        window.draw(Alt_F4_Button);
        window.draw(Alt_F4_ButtonText);
        window.draw(horizontalscrollBar1Background);
        window.draw(horizontalscrollBar1);
        window.draw(verticalscrollBar1Background);
        window.draw(verticalscrollBar1);
        window.draw(horizontalscrollBar2Background);
        window.draw(horizontalscrollBar2);
        window.draw(verticalscrollBar2Background);
        window.draw(verticalscrollBar2);


        for (int k = 0; k < numButtons; k++)
        {
            window.draw(buttons[k]);

        }
        window.draw(menuBar6);
        window.draw(Icons2);
        window.draw(textBox);
        window.draw(text);


        if (F3_Clicked || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3)
        {
            F3_Clicked = false;
            openFile(lastClick);

        }

        if (F4_Clicked || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F4)
        {
            F4_Clicked = false;
            openFile(lastClick);

        }


        if (F5_Clicked == true || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F5)

        {
            copyFile(lastClick, moveAt);
            for (k = 1; k <= totalFilesSection1; k++)
            {
                D[k].director = false;
                D[k].print = false;
                v1[k] = 0;
            }
            for (k = 1; k <= totalFilesSection2; k++)
            {
                C[k].director = false;
                C[k].print = false;
                v2[k] = 0;
            }
            totalFilesSection1 = 0;
            totalFilesSection2 = 0;
            FileNode* root = createFileTree(pathDirector1);
            FileNode* root1 = createFileTree(pathDirector2);
            traverseDFS_FirstPath(root, totalFilesSection1, p);
            traverseDFS_SecondPath(root1, totalFilesSection2, p);
            F5_Clicked = false;
            sf::Vector2f mousePos(0, 0);

        }

        if (F6_Clicked || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F6)
        {
            F6_Clicked = false;
            moveAt = moveAt + "\\" + getNameFromPath(lastClick);
            moveFile(lastClick, moveAt);
            for (k = 1; k <= totalFilesSection1; k++)
            {
                D[k].director = false;
                D[k].print = false;
                v1[k] = 0;
            }
            for (k = 1; k <= totalFilesSection2; k++)
            {
                C[k].director = false;
                C[k].print = false;
                v2[k] = 0;
            }
            totalFilesSection1 = 0;
            totalFilesSection2 = 0;
            FileNode* root = createFileTree(pathDirector1);
            FileNode* root1 = createFileTree(pathDirector2);
            traverseDFS_FirstPath(root, totalFilesSection1, p);
            traverseDFS_SecondPath(root1, totalFilesSection2, p);


        }

        window.setFramerateLimit(4);

        if (F7_Clicked || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F7)
        {
            F7_Clicked = false;
            moveAt = moveAt + "\\NewFolder";
            createFile(moveAt);
            for (k = 1; k <= totalFilesSection1; k++)
            {
                D[k].director = false;
                D[k].print = false;
                v1[k] = 0;
            }
            for (k = 1; k <= totalFilesSection2; k++)
            {
                C[k].director = false;
                C[k].print = false;
                v2[k] = 0;
            }
            totalFilesSection1 = 0;
            totalFilesSection2 = 0;
            FileNode* root = createFileTree(pathDirector1);
            FileNode* root1 = createFileTree(pathDirector2);
            traverseDFS_FirstPath(root, totalFilesSection1, p);
            traverseDFS_SecondPath(root1, totalFilesSection2, p);


        }

        window.setFramerateLimit(7);

        if (F8_Clicked || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F8)
        {
            F8_Clicked = false;
            deleteFile(lastClick);
            for (k = 1; k <= totalFilesSection1; k++)
            {
                D[k].director = false;
                D[k].print = false;
                v1[k] = 0;
            }
            for (k = 1; k <= totalFilesSection2; k++)
            {
                C[k].director = false;
                C[k].print = false;
                v2[k] = 0;
            }
            totalFilesSection1 = 0;
            totalFilesSection2 = 0;
            FileNode* root = createFileTree(pathDirector1);
            FileNode* root1 = createFileTree(pathDirector2);
            traverseDFS_FirstPath(root, totalFilesSection1, p);
            traverseDFS_SecondPath(root1, totalFilesSection2, p);


        }

        window.display();

    }

    return 0;
}
