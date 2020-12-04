#include "DomainPoints.h"

DomainPoints::DomainPoints()
{
}

DomainPoints::DomainPoints(std::vector<Point> pointList, Point min, Point max)
{
    this->domainPoints = getInteriorPoints(pointList, min, max);
}

DomainPoints::~DomainPoints()
{
}

std::vector<PointDouble> DomainPoints::getInteriorPoints(std::vector<Point> pointList, Point min, Point max)
{
    std::vector<PointDouble> output;
    //criar uma região ==> no fututo, fazer um constructor que já aceite uma região
    GraphicsPath path;
    path.StartFigure();
    std::vector<Point>::iterator it = pointList.begin();
    Point p1 = pointList.back();
    for (it = pointList.begin(); it != pointList.end(); it++) {
        Point p2 = *it;
        path.AddLine(p1, p2);
        p1 = p2;
    }
    Region region1(&path);
    //aqui vai um teste de mover a região pra ver se dá jeito nessa merda
    PointF pTranslate(0.5, 0.5);

    //aqui é um bom lugar pra testar o openMP ==> mas ele está bem mais lento
    double t0 = omp_get_wtime();
    for (int line = min.Y; line < max.Y; line++) {
        for (int collum = min.X; collum < max.X; collum++) {
            PointF pf((collum+0.5), (line+0.5));
            PointDouble pd((collum + 0.5), (line + 0.5));
            if (region1.IsVisible(pf- pTranslate)) {
                output.push_back(pd);
            }
        }
    }
    //omp_set_num_threads(4);
    //#pragma omp parallel
    //{
    //    //std::vector<PointF> temp;
    //    Region region2(&path);
    //    for (int line = min.Y; line < max.Y; line++) {
    //        #pragma omp for nowait schedule(static)
    //            for(int collum = min.X; collum < max.X; collum++) {
    //                PointF pf((collum + 0.5), (line + 0.5));
    //                if (region2.IsVisible(pf)) {
    //                    #pragma omp critical
    //                    {
    //                        output.push_back(pf);
    //                    }
    //                }
    //             }
    //    }
    //    //#pragma omp critical
    //    //{
    //    //    output.insert(output.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
    //    //}
    //}
    double t1 = omp_get_wtime();
    std::cout << "time to create points = " << (t1 - t0) << " s" << std::endl;

    return output;
}
