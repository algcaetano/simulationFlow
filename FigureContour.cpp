#include "FigureContour.h"

FigureContour::FigureContour()
{
}

FigureContour::FigureContour(string fileName)
    :RgbaPNG(fileName)
{
    //vetor com as coordenadas (x,y) do contorno
    list<Point> coordContorno;
    getContour(this->rgba, this->Width, this->Height, this->stride, this->bytesPerPixel, coordContorno);
    //ordenar e pegar a cor dos pontos
    list<list <Point> ::iterator> OtmPath = findPath(coordContorno);
    //ordenar os pontos do contorno e a ordem das cores
    pointList = pathToPointList(OtmPath);
    pixelColor = getColor(this->pointList, *this);
    //Remover os pontos desnecessários para criar um polígono
    pointList = pointToPol(pointList, pixelColor);
    pixelColor = getColor(this->pointList, *this);
    this->limits = getLimits(this->pointList);
}

FigureContour::~FigureContour()
{
}

void FigureContour::getContour(vector<BYTE>& input, int width, int height, int stride, int bytesPerPixel, list<Point>& output)
{
    for (int i = 0; i < width;i++) {//colunas
        for (int j = 0;j < height;j++) {//linhas
            if (isBorder(input, i, j, width, height, stride, bytesPerPixel)) {
                output.push_back(Point::Point(i, j));
                //int initialPos = j * stride + i * bytesPerPixel;
                //for (int k = 0;k < bytesPerPixel;k++) {
                    //input[initialPos + k] = 0;
                    //output.push_back(input[initialPos + k]); //cor do pixel
                //}
            }
        }
    }
}

int FigureContour::pixelValue(vector<BYTE>& input, int x, int y, int width, int height, int stride, int bytesPerPixel)
{
    if (x > (width - 1) || y > (height - 1)) {
        cerr << "Fora dos limites";
        EXIT_FAILURE;
    }
    else {
        long long int initialPos = (long long int)y * stride + (long long int)x * bytesPerPixel;
        int pixelValue = input[initialPos] + 256 * input[initialPos + 1] + 256 * 256 * input[initialPos + 2];
        return pixelValue;
    }
}

bool FigureContour::isBorder(vector<BYTE>& input, int x, int y, int width, int height, int stride, int bytesPerPixel)
{
    int white = 256 * 256 * 256 - 1;
    if (pixelValue(input, x, y, width, height, stride, bytesPerPixel) == white) {
        return false;
    }
    else {
        bool aux = false;
        //ver se algum vizinho é um pixel branco
        aux = aux || (pixelValue(input, minValue((x + 1), (width - 1)), y, width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, minValue((x + 1), (width - 1)), maxValue((y - 1), 0), width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, x, maxValue((y - 1), 0), width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, maxValue((x - 1), 0), maxValue((y - 1), 0), width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, maxValue((x - 1), 0), y, width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, maxValue((x - 1), 0), minValue((y + 1), (height - 1)), width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, x, minValue((y + 1), (height - 1)), width, height, stride, bytesPerPixel) == white);
        aux = aux || (pixelValue(input, minValue((x + 1), (width - 1)), minValue((y + 1), (height - 1)), width, height, stride, bytesPerPixel) == white);
        if ((x == 0) | (x == (width - 1)) | (y == 0) | (y == (height - 1))) {//se estiver na borda da figura
            aux = true;
        }
        return aux;
    }
}

int FigureContour::maxValue(int x, int y)
{
    if (x > y) {
        return x;
    }
    else {
        return y;
    }
}

int FigureContour::minValue(int x, int y)
{
    if (x < y) {
        return x;
    }
    else {
        return y;
    }
}

double FigureContour::pointDistance(Point p1, Point p2)
{
    Point r = p2 - p1;
    return sqrt((r.X) * (r.X) + (r.Y) * (r.Y));
}

list<list<Point>::iterator> FigureContour::findPath(list<Point>& input)
{
    //************Variáveis****************//
    list<list <Point> ::iterator> otmPath; //armazenar aqui o resultado final
    list<list <Point>::iterator> pointList = getOrder(input); //lista de pontos inicial
    list<list<Point>::iterator> currentPath; //aqui serão armazendados os endereçoes dos pontos
    list<list<list<Point>::iterator>::iterator> bifurcsInPath; //aqui serão armazenadas as bifurcações que ocorrem ao longo de currentaPath
    list< list<list<Point>::iterator> > bifurcPointsAddress; //aqui serão armazenados os pontos possíveis em cada bifurcação
    list<double> distances; //a distância percorrida até o ponto em questão
    list<list<double>::iterator> bifurcsDistances; //distâncias até as bifurcações
    double totalDistance = 0.0; //distância total percorrida no caminho

    //**************Inicialização*************//
    list<list <Point> ::iterator> ::iterator it = pointList.begin(); //começo da lista de pontos
    currentPath.push_back(*it); //copiar primeiro ponto da lista de pontos para o caminho
    pointList.pop_front(); //remover o ponto adicionado da lista de pontos
    distances.push_back(0.0); //começar com a distância zero
    Point initialPoint = *(currentPath.back()); //definir o ponto adicionado como ponto de referência
    double currentDistance = pointDistance(initialPoint, **pointList.begin()); //antecipar a distância que será calculada
    list<list <Point> ::iterator> ::iterator currentPathIndex = currentPath.begin(); //atual ponto na lista (caminho)
    list<double>::iterator currentDistanceIndex = distances.begin(); //atual distância

    //***********atualização*************//
    double inf = std::numeric_limits<double>::infinity(); //distância ótima é infinita na primeira iteração

    newPath(pointList, initialPoint, currentDistance, currentPath, totalDistance, distances, bifurcPointsAddress,
        bifurcsInPath, currentPathIndex, bifurcsDistances, currentDistanceIndex, inf);

    otmPath = currentPath; //primeiro caminho encontrado
    double otmMin = distances.back(); //ditância percorrida nesse caminho
    double minPoss = (double)currentPath.size() - 1; //distância mínima possível

    bool isbifurc = !(bifurcPointsAddress.size() == 0);//pode não ter nenhuma bifurcação

    if (isbifurc) {
        while ((bifurcPointsAddress.back()).size() > 0 && bifurcPointsAddress.size() > 1 && !compareDouble(otmMin, minPoss)) {//última bifurcaçao
            double oldTotalDistance = totalDistance; //distância percorrida no caminho anterior

            list<list <Point> ::iterator> ::iterator lastBifurcation = bifurcsInPath.back(); //aqui teve a última bifurcação em currentPath
            lastBifurcation++; //ponto adicionado após a bifurcação
            list<double>::iterator lastBifurcationDistance = bifurcsDistances.back(); //aqui é a distância até a bifurcação
            lastBifurcationDistance++; //a partir daqui são as distâncias após a bifurcação
            for (list<list <Point> ::iterator> ::iterator it = lastBifurcation; it != currentPath.end(); it++) {
                pointList.push_back(*it); //nova lista de pontos
            }
            currentPath.erase(lastBifurcation, currentPath.end()); //novo caminho a ser percorrido sem os pontos após a bifurcação
            distances.erase(lastBifurcationDistance, distances.end()); //novas distâncias sem os pontos após a bifurcação
            //aqui tem que adicionar o próximo ponto da bifurcação e apagar ele
            Point p1 = *currentPath.back(); //ponto final
            currentPath.push_back((bifurcPointsAddress.back()).front()); //primeiro elemento da última lista
            pointList.remove((bifurcPointsAddress.back()).front()); //remover da lista de pontos
            Point p2 = *currentPath.back(); //pontoadicionado
            //calcular a nova distância adicionada e adicionar no vetor de distâncias
            distances.push_back(pointDistance(p1, p2) + distances.back());
            if ((bifurcPointsAddress.back()).size() == 1) { //se tiver só um elemento, apagar a última lista
                bifurcPointsAddress.pop_back();
                bifurcsInPath.pop_back(); //também não é mais um ponto de bifurcação
                bifurcsDistances.pop_back(); //a distância agora mudou
            }
            else {
                (bifurcPointsAddress.back()).pop_front(); //se tiver mais de um, remover somente o primeiro da lista
            }
            initialPoint = *currentPath.back(); //novo ponto inicial (último ponto do caminho)
            currentDistance = pointDistance(initialPoint, **pointList.begin()); //antecipar a distância que será calculada
            totalDistance = distances.back(); //distância total a ser comparada com a distância total anterior
            currentPathIndex = currentPath.end();
            currentPathIndex--;
            currentDistanceIndex = distances.end();
            currentDistanceIndex--;

            newPath(pointList, initialPoint, currentDistance, currentPath, totalDistance, distances, bifurcPointsAddress,
                bifurcsInPath, currentPathIndex, bifurcsDistances, currentDistanceIndex, otmMin);

            if (totalDistance < oldTotalDistance) { //se a distância percorrida agora é menor que a antiga
                otmPath = currentPath; //caminho ótimo é esse caminho recém calculado
                otmMin = distances.back(); //distância mínima a ser batida mudou pra essa nova
            }
            else {//caminho ótimo não mudou
                totalDistance = oldTotalDistance; //a distância a ser comparada ainda é a antiga
            }
        }
    }
    return otmPath;
}

list<list<Point>::iterator> FigureContour::getOrder(list<Point>& pointList)
{
    list<list <Point> ::iterator> output;
    for (list <Point> ::iterator it = pointList.begin(); it != pointList.end();++it) {
        output.push_back(it); //endereços, na sequência, da lista de entrada
    }
    return output;
}

bool FigureContour::compareDouble(double num1, double num2)
{
    double val = sqrt((num1 - num2) * (num1 - num2)); //módulo da diferença
    if (val < 1e-15) {
        return true;
    }
    else {
        return false;
    }
}

void FigureContour::newPath(list<list<Point>::iterator>& pointList, Point& initialPoint,
    double& currentDistance, list<list<Point>::iterator>& currentPath, double& totalDistance,
    list<double>& distances, list<list<list<Point>::iterator>>& bifurcPointsAddress,
    list<list<list<Point>::iterator>::iterator>& bifurcsInPath, list<list<Point>::iterator>::iterator& currentPathIndex,
    list<list<double>::iterator>& bifurcsDistances, list<double>::iterator& currentDistanceIndex, double& currentMin)
{
    while (!pointList.empty()) {//enquanto houver pontos na lista de pontos ==> aqui tem que colocar mais uma condição pra comparar com a distância ótima
        list<list<Point>::iterator> tempBifurcsInPath; //variável para armazenar os pontos possíveis após a bifurcação
        for (list<list <Point> ::iterator> ::iterator it = pointList.begin(); it != pointList.end(); it++) {//percorrer todos os pontos
            double distance = pointDistance(initialPoint, **it); //calcular a distância
            if (compareDouble(distance, currentDistance)) {//comparar as distâncias dentro da precisão da máquina
                tempBifurcsInPath.push_back(*it); //se tiver a mesma distância, armazenar
            }
            else if (distance < currentDistance) {//Se encontrar um ponto ainda mais próximo
                tempBifurcsInPath.clear(); //limpar os índices anteriores
                tempBifurcsInPath.push_back(*it); //adicionar o novo ponto
                currentDistance = distance; //nova distância a ser testada
            }
        }

        currentPath.push_back(*tempBifurcsInPath.begin()); //adicionar o endereço do primeiro ponto em index
        tempBifurcsInPath.pop_front(); //remover o ponto adicionado
        totalDistance += currentDistance; //atualizar a distância total
        distances.push_back(totalDistance); //adicionar a distância atual

        if (tempBifurcsInPath.size() > 0) {//Se tiver mais de um ponto na lista, tem uma bifurcação
            bifurcPointsAddress.push_back(tempBifurcsInPath); //adicionar as bifurcações (os endereços dos pontos que ainda não foram tomados)
            bifurcsInPath.push_back(currentPathIndex); //ponto onde ocorre a bifurcação dentro do caminho
            bifurcsDistances.push_back(currentDistanceIndex); //ponto na lista de distâncias onde ocorre a bifurcação
        }

        currentPathIndex++; //atualizar a posição em que se encontra a currentPath
        currentDistanceIndex++; //atualizar a posição na lista de distâncias
        pointList.remove(*currentPathIndex); //remover da lista de pontos possíveis o ponto adicionado
        initialPoint = *currentPath.back(); //novo ponto em relação ao qual se calcula as distâncias
        tempBifurcsInPath.clear(); //limpar os índices

        if (isExceed(distances.back() + pointList.size(), currentMin)) {//não tem como alcançar a distância mínima mais
            currentPath.splice(currentPath.end(), pointList); //completar o caminho com os pontos restantes
            totalDistance = currentMin + 1; //garantir que a distância total desse caminho seja maior que a ótima
        }

        if (!pointList.empty()) {//checar se não se esgotaram os pontos
            currentDistance = pointDistance(initialPoint, **pointList.begin()); //distância inicial => em relação ao primeiro ponto da lista
        }
    }
}

vector<Point> FigureContour::pathToPointList(list<list<Point>::iterator> input)
{
    vector<Point> output;
    for (list<list<Point>::iterator>::iterator it = input.begin(); it != input.end();it++) {
        output.push_back(**it);
    }
    return output;
}

bool FigureContour::isExceed(double minPossible, double actualMin) {
    if (minPossible > actualMin) {
        return true;
    }
    else {
        return false;
    }
}

vector<Color> FigureContour::getColor(vector<Point>& pointList, RgbaPNG& png)
{
    vector<Color>  pixelColor;

    //para criar um bitmap tem que inicilizar o gdi+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Bitmap bmp(
        png.Width,
        png.Height,
        png.stride,
        png.format,
        png.rgba.data()
    );

    Status test = bmp.GetLastStatus(); //aqui tem estar ok

    Color currentPixelColor;

    for (vector<Point>::iterator it = pointList.begin(); it != pointList.end();it++) {
        bmp.GetPixel((*it).X, (*it).Y, &currentPixelColor);
        pixelColor.push_back(currentPixelColor);
    }

    //GdiplusShutdown(gdiplusToken);

    return pixelColor;
}

bool FigureContour::isColinear(Point& p1, Point& p2, Point& p3, Color c1, Color c2, Color c3) {

    bool xDir = false;
    bool yDir = false;
    bool colors = false;

    if (p1.X == p2.X && p2.X == p3.X) {//colinear em x
        xDir = true;
    }

    if (p1.Y == p2.Y && p2.Y == p3.Y) {//colinear em y
        yDir = true;
    }

    if (c1.GetValue() == c2.GetValue() && c2.GetValue() == c3.GetValue()) {// mesmas cores
        colors = true;
    }

    return ((xDir & colors) | (yDir & colors)); //cores colineares
}

vector<Point> FigureContour::getLimits(vector<Point>& pointList)
{
    vector<Point> limits;

    //set the limits to the first point
    int minX = pointList.front().X;
    int maxX = pointList.front().X;
    int minY = pointList.front().Y;
    int maxY = pointList.front().Y;

    for (vector<Point>::iterator it = pointList.begin(); it != pointList.end(); it++) {
        if ((*it).X < minX) {
            minX = (*it).X;
        }
        if ((*it).X > maxX) {
            maxX = (*it).X;
        }
        if ((*it).Y < minY) {
            minY = (*it).Y;
        }
        if ((*it).Y > maxY) {
            maxY = (*it).Y;
        }
    }

    Point p1(minX, minY), p2(maxX, maxY);
    limits.push_back(p1);
    limits.push_back(p2);

    return limits;
}

vector<Point> FigureContour::pointToPol(vector<Point>& input, vector<Color>& inputColor) {
    vector<Point> output;
    vector<Point>::iterator it = input.begin();
    Point p1 = *it;
    it++;
    Point p2 = *it;
    it++;
    vector<Color>::iterator colIt = inputColor.begin();
    Color c1 = *colIt;
    colIt++;
    Color c2 = *colIt;
    colIt++;

    for (it; it != input.end();it++) {
        Point p3 = *it;
        Color c3 = *colIt;

        if (isColinear(p1, p2, p3, c1, c2, c3)) {//apagar o ponto do meio
            p2 = p3;
            c2 = c3;
        }
        else {//armazenar o primeiro ponto e apagar ele
            output.push_back(p1);
            p1 = p2;
            c1 = c2;
            p2 = p3;
            c2 = c3;
        }
        colIt++;
    }
    //sobraram dois pontos na lista => tem que colocar o primeiro ponto como o p3 e depois o segundo ponto
    it = input.begin();
    colIt = inputColor.begin();
    for (int i = 0;i < 2;i++) {
        Point p3 = *it;
        Color c3 = *colIt;
        if (isColinear(p1, p2, p3, c1, c2, c3)) {//apagar o ponto do meio
            p2 = p3;
            c2 = c3;
        }
        else {//armazenar o primeiro ponto e apagar ele
            output.push_back(p1);
            p1 = p2;
            c1 = c2;
            p2 = p3;
            c2 = c3;
        }
        it++;
        colIt++;
    }
    //output.push_back(p1);
    //output.push_back(p2);
    return output;

}