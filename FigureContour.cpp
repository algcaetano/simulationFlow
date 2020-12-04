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
    //Remover os pontos desnecess�rios para criar um pol�gono
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
        //ver se algum vizinho � um pixel branco
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
    //************Vari�veis****************//
    list<list <Point> ::iterator> otmPath; //armazenar aqui o resultado final
    list<list <Point>::iterator> pointList = getOrder(input); //lista de pontos inicial
    list<list<Point>::iterator> currentPath; //aqui ser�o armazendados os endere�oes dos pontos
    list<list<list<Point>::iterator>::iterator> bifurcsInPath; //aqui ser�o armazenadas as bifurca��es que ocorrem ao longo de currentaPath
    list< list<list<Point>::iterator> > bifurcPointsAddress; //aqui ser�o armazenados os pontos poss�veis em cada bifurca��o
    list<double> distances; //a dist�ncia percorrida at� o ponto em quest�o
    list<list<double>::iterator> bifurcsDistances; //dist�ncias at� as bifurca��es
    double totalDistance = 0.0; //dist�ncia total percorrida no caminho

    //**************Inicializa��o*************//
    list<list <Point> ::iterator> ::iterator it = pointList.begin(); //come�o da lista de pontos
    currentPath.push_back(*it); //copiar primeiro ponto da lista de pontos para o caminho
    pointList.pop_front(); //remover o ponto adicionado da lista de pontos
    distances.push_back(0.0); //come�ar com a dist�ncia zero
    Point initialPoint = *(currentPath.back()); //definir o ponto adicionado como ponto de refer�ncia
    double currentDistance = pointDistance(initialPoint, **pointList.begin()); //antecipar a dist�ncia que ser� calculada
    list<list <Point> ::iterator> ::iterator currentPathIndex = currentPath.begin(); //atual ponto na lista (caminho)
    list<double>::iterator currentDistanceIndex = distances.begin(); //atual dist�ncia

    //***********atualiza��o*************//
    double inf = std::numeric_limits<double>::infinity(); //dist�ncia �tima � infinita na primeira itera��o

    newPath(pointList, initialPoint, currentDistance, currentPath, totalDistance, distances, bifurcPointsAddress,
        bifurcsInPath, currentPathIndex, bifurcsDistances, currentDistanceIndex, inf);

    otmPath = currentPath; //primeiro caminho encontrado
    double otmMin = distances.back(); //dit�ncia percorrida nesse caminho
    double minPoss = (double)currentPath.size() - 1; //dist�ncia m�nima poss�vel

    bool isbifurc = !(bifurcPointsAddress.size() == 0);//pode n�o ter nenhuma bifurca��o

    if (isbifurc) {
        while ((bifurcPointsAddress.back()).size() > 0 && bifurcPointsAddress.size() > 1 && !compareDouble(otmMin, minPoss)) {//�ltima bifurca�ao
            double oldTotalDistance = totalDistance; //dist�ncia percorrida no caminho anterior

            list<list <Point> ::iterator> ::iterator lastBifurcation = bifurcsInPath.back(); //aqui teve a �ltima bifurca��o em currentPath
            lastBifurcation++; //ponto adicionado ap�s a bifurca��o
            list<double>::iterator lastBifurcationDistance = bifurcsDistances.back(); //aqui � a dist�ncia at� a bifurca��o
            lastBifurcationDistance++; //a partir daqui s�o as dist�ncias ap�s a bifurca��o
            for (list<list <Point> ::iterator> ::iterator it = lastBifurcation; it != currentPath.end(); it++) {
                pointList.push_back(*it); //nova lista de pontos
            }
            currentPath.erase(lastBifurcation, currentPath.end()); //novo caminho a ser percorrido sem os pontos ap�s a bifurca��o
            distances.erase(lastBifurcationDistance, distances.end()); //novas dist�ncias sem os pontos ap�s a bifurca��o
            //aqui tem que adicionar o pr�ximo ponto da bifurca��o e apagar ele
            Point p1 = *currentPath.back(); //ponto final
            currentPath.push_back((bifurcPointsAddress.back()).front()); //primeiro elemento da �ltima lista
            pointList.remove((bifurcPointsAddress.back()).front()); //remover da lista de pontos
            Point p2 = *currentPath.back(); //pontoadicionado
            //calcular a nova dist�ncia adicionada e adicionar no vetor de dist�ncias
            distances.push_back(pointDistance(p1, p2) + distances.back());
            if ((bifurcPointsAddress.back()).size() == 1) { //se tiver s� um elemento, apagar a �ltima lista
                bifurcPointsAddress.pop_back();
                bifurcsInPath.pop_back(); //tamb�m n�o � mais um ponto de bifurca��o
                bifurcsDistances.pop_back(); //a dist�ncia agora mudou
            }
            else {
                (bifurcPointsAddress.back()).pop_front(); //se tiver mais de um, remover somente o primeiro da lista
            }
            initialPoint = *currentPath.back(); //novo ponto inicial (�ltimo ponto do caminho)
            currentDistance = pointDistance(initialPoint, **pointList.begin()); //antecipar a dist�ncia que ser� calculada
            totalDistance = distances.back(); //dist�ncia total a ser comparada com a dist�ncia total anterior
            currentPathIndex = currentPath.end();
            currentPathIndex--;
            currentDistanceIndex = distances.end();
            currentDistanceIndex--;

            newPath(pointList, initialPoint, currentDistance, currentPath, totalDistance, distances, bifurcPointsAddress,
                bifurcsInPath, currentPathIndex, bifurcsDistances, currentDistanceIndex, otmMin);

            if (totalDistance < oldTotalDistance) { //se a dist�ncia percorrida agora � menor que a antiga
                otmPath = currentPath; //caminho �timo � esse caminho rec�m calculado
                otmMin = distances.back(); //dist�ncia m�nima a ser batida mudou pra essa nova
            }
            else {//caminho �timo n�o mudou
                totalDistance = oldTotalDistance; //a dist�ncia a ser comparada ainda � a antiga
            }
        }
    }
    return otmPath;
}

list<list<Point>::iterator> FigureContour::getOrder(list<Point>& pointList)
{
    list<list <Point> ::iterator> output;
    for (list <Point> ::iterator it = pointList.begin(); it != pointList.end();++it) {
        output.push_back(it); //endere�os, na sequ�ncia, da lista de entrada
    }
    return output;
}

bool FigureContour::compareDouble(double num1, double num2)
{
    double val = sqrt((num1 - num2) * (num1 - num2)); //m�dulo da diferen�a
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
    while (!pointList.empty()) {//enquanto houver pontos na lista de pontos ==> aqui tem que colocar mais uma condi��o pra comparar com a dist�ncia �tima
        list<list<Point>::iterator> tempBifurcsInPath; //vari�vel para armazenar os pontos poss�veis ap�s a bifurca��o
        for (list<list <Point> ::iterator> ::iterator it = pointList.begin(); it != pointList.end(); it++) {//percorrer todos os pontos
            double distance = pointDistance(initialPoint, **it); //calcular a dist�ncia
            if (compareDouble(distance, currentDistance)) {//comparar as dist�ncias dentro da precis�o da m�quina
                tempBifurcsInPath.push_back(*it); //se tiver a mesma dist�ncia, armazenar
            }
            else if (distance < currentDistance) {//Se encontrar um ponto ainda mais pr�ximo
                tempBifurcsInPath.clear(); //limpar os �ndices anteriores
                tempBifurcsInPath.push_back(*it); //adicionar o novo ponto
                currentDistance = distance; //nova dist�ncia a ser testada
            }
        }

        currentPath.push_back(*tempBifurcsInPath.begin()); //adicionar o endere�o do primeiro ponto em index
        tempBifurcsInPath.pop_front(); //remover o ponto adicionado
        totalDistance += currentDistance; //atualizar a dist�ncia total
        distances.push_back(totalDistance); //adicionar a dist�ncia atual

        if (tempBifurcsInPath.size() > 0) {//Se tiver mais de um ponto na lista, tem uma bifurca��o
            bifurcPointsAddress.push_back(tempBifurcsInPath); //adicionar as bifurca��es (os endere�os dos pontos que ainda n�o foram tomados)
            bifurcsInPath.push_back(currentPathIndex); //ponto onde ocorre a bifurca��o dentro do caminho
            bifurcsDistances.push_back(currentDistanceIndex); //ponto na lista de dist�ncias onde ocorre a bifurca��o
        }

        currentPathIndex++; //atualizar a posi��o em que se encontra a currentPath
        currentDistanceIndex++; //atualizar a posi��o na lista de dist�ncias
        pointList.remove(*currentPathIndex); //remover da lista de pontos poss�veis o ponto adicionado
        initialPoint = *currentPath.back(); //novo ponto em rela��o ao qual se calcula as dist�ncias
        tempBifurcsInPath.clear(); //limpar os �ndices

        if (isExceed(distances.back() + pointList.size(), currentMin)) {//n�o tem como alcan�ar a dist�ncia m�nima mais
            currentPath.splice(currentPath.end(), pointList); //completar o caminho com os pontos restantes
            totalDistance = currentMin + 1; //garantir que a dist�ncia total desse caminho seja maior que a �tima
        }

        if (!pointList.empty()) {//checar se n�o se esgotaram os pontos
            currentDistance = pointDistance(initialPoint, **pointList.begin()); //dist�ncia inicial => em rela��o ao primeiro ponto da lista
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