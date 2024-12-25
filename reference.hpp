#include "main.h"
#include "raster.h"
#include "geometry.h"
#include "bitmap.h"
#include <stdio.h>
//-------------------------------------------------------------------------
// Размеры блока
const long TileWidth = 32;
const long TileHeight = 32;
//-------------------------------------------------------------------------
// Отображение полигона с проверкой глубины по блочному Z-буферу
// in_Poly – список вершин полигона
// in_color – цвет полигона
// in_tile_offset_x, in_tile_offset_y – смещение блока в окне
// in_zbuffer – массив Z-буфера
void RasterPolygonTileZBuffer(
TCircleList<TVertex2D> &in_Poly,
short in_color,
long in_tile_offset_x,
long in_tile_offset_y,
float *in_zbuffer)
{
TEdgeList *Edge1=0,*Edge2=0;
// Занести данные рёбер полигона в списки строк растра
long ys,ye;
PrepareRasterPolygon(in_Poly,0,ys,ye);
for(long y=ys;y<ye;y++)
{
//проверка, что рёбра завершились
if (Edge1 && (y>=Edge1->end)) SafeDelete(Edge1);
if (Edge2 && (y>=Edge2->end)) SafeDelete(Edge2);
//обновление координат для очередной строки
if (Edge1) { Edge1->x+=Edge1->dx; Edge1->w+=Edge1->dw; }
if (Edge2) { Edge2->x+=Edge2->dx; Edge2->w+=Edge2->dw; }
//добавление новых рёбер
while(LineEdges[y])
{
if (!Edge1) Edge1=LineEdges[y];
else if (!Edge2) Edge2=LineEdges[y];
else assert(0,"Больше двух рёбер на строку");
LineEdges[y]=LineEdges[y]->next;
}
assert(Edge1 && Edge2,"Меньше двух рёбер на строку");
if (!Edge1 || !Edge2) continue;
// Если спан не попадает в блок - не рисуем его
if (y < in_tile_offset_y) continue;
if (in_tile_offset_y + TileHeight < y) continue;
// Edge1 должно быть всегда слева от Edge2
if (Edge1->x>Edge2->x) Swap(Edge1,Edge2);
// Начало и конец спана
long xs=(long)floor(Edge1->x+0.5);
long xe=(long)floor(Edge2->x+0.5);
// Проверка на выход за пределы растра
if (xs<0) xs=0;
if (xe>RWidth) xe=RWidth;
// Если спан выходит за границы блока - ограничиваем его
if (xs<in_tile_offset_x) xs=in_tile_offset_x;
if (xe>in_tile_offset_x + TileWidth)
xe=in_tile_offset_x + TileWidth;
// Спан вне растра
if (xs>=xe) continue;
//рисуем спан
float dw=(Edge2->w-Edge1->w)/(Edge2->x-Edge1->x);
float w=Edge1->w+dw*(xs+0.5f-Edge1->x);
for(long x=xs;x<xe;x++)
{
// сравниваем глубину со значением в z-буфере
long zbuffer_offset = (y - in_tile_offset_y) * TileWidth +
(x - in_tile_offset_x);
if (in_zbuffer[zbuffer_offset] < w)
{
// заносим глубину
in_zbuffer[zbuffer_offset] = w;
// рисуем пиксел
PutPixel(x,y,in_color);
}
//обновляем w-координату для следующего пиксела
w+=dw;
}
}
SafeDelete(Edge1);
SafeDelete(Edge2);
}
//-------------------------------------------------------------------------
// Структура для хранения данных о проекции полигона
struct TPolygonInfo
{
TCircleList<TVertex2D> Corners;
short Color;
};
//-------------------------------------------------------------------------
// Нарисовать сцену c использованием блочного Z-Буфера
void DrawSceneTileZBuffer()
{
// Очистить изображение и все дополнительные данные в начале кадра
InitRasterFrame();
// Плоскость отсечения полигонов за камерой (ближняя плоскость)
TPlane plane(0,0,-1,-MINIMUM_Z);
// Количество блоков в окне
long tile_size_x = (RWidth + TileWidth - 1) / TileWidth;
long tile_size_y = (RHeight + TileHeight - 1) / TileHeight;
// Списки полигонов, попавших в блоки
TCircleList<TPolygonInfo> *tile_polygones =
new TCircleList<TPolygonInfo>[tile_size_x * tile_size_y];
// Список всех спроецированых полигонов
TCircleList<TPolygonInfo> projected_polygones;
// Массив Z-буфера
float ZBuffer[TileWidth * TileHeight];
// Заносим все грани в списки соответствующих блоков
for(long i=0;i<NumberFaces;i++)
{
const TFace &face=Faces[i];
// Формируем полигон в пространстве
TCircleList<TVertex3D> Poly3D;
Poly3D.append(new TVertex3D(Vertices3D[face.vertex[0]]));
Poly3D.append(new TVertex3D(Vertices3D[face.vertex[1]]));
Poly3D.append(new TVertex3D(Vertices3D[face.vertex[2]]));
// Отсекаем по ближней плоскости
if (TruncPolygon3D(Poly3D,plane))
{
// Создаём объект для хранения полигона
TPolygonInfo *Poly2D = new TPolygonInfo();
// Заносим цвет полигона
Poly2D->Color = face.color_id;
// Проецируем полигон на экран
ProjectPolygon(Poly3D, Poly2D->Corners);
// Заносим полигон в список, чтобы в конце удалить
projected_polygones.append(Poly2D);
// Рассчитываем координаты прямоугольника,
// описанного вокруг полигона
TVector2D min_rect(1e20f, 1e20f), max_rect(-1e20f);
for(TCircleListIterator<TVertex2D> j(Poly2D->Corners);
!j.end();j.step())
{
const TVertex2D *vertex = j.get();
if (vertex->x < min_rect.x) min_rect.x = vertex->x;
if (vertex->x > max_rect.x) max_rect.x = vertex->x;
if (vertex->y < min_rect.y) min_rect.y = vertex->y;
if (vertex->y > max_rect.y) max_rect.y = vertex->y;
}
// Заносим полигон в списки соответствующих блоков
long tile_min_x = Max((long)floor(min_rect.x / TileWidth), 0L);
long tile_max_x = Min((long)floor(max_rect.x / TileWidth),
tile_size_x - 1L);
long tile_min_y = Max((long)floor(min_rect.y / TileHeight), 0L);
long tile_max_y = Min((long)floor(max_rect.y / TileHeight),
tile_size_y - 1L);
for(long tile_y=tile_min_y;tile_y<=tile_max_y;++tile_y)
for(long tile_x=tile_min_x;tile_x<=tile_max_x;++tile_x)
tile_polygones[tile_y*tile_size_x+tile_x].append(Poly2D);
}
}
// Для каждого блока
for(long tile_y = 0; tile_y < tile_size_y; ++tile_y)
for(long tile_x = 0; tile_x < tile_size_x; ++tile_x)
{
// Границы блока
long tile_min_x = tile_x * TileWidth;
long tile_max_x = Min((tile_x + 1) * TileWidth, RWidth);
long tile_min_y = tile_y * TileHeight;
long tile_max_y = Min((tile_y + 1) * TileHeight, RHeight);
// Прямые для отсечения по границам блока
TLine left(-1,0,-tile_min_x), right(1,0,tile_max_x);
TLine top(0,-1,-tile_min_y), bottom(0,1,tile_max_y);
// Заносим в Z-буфер максимальную глубину
for(long i = 0; i < TileWidth * TileHeight; ++i)
ZBuffer[i] = 0.0f;
// Рисуем все полигоны, попавшие в список блока
TCircleList<TPolygonInfo> &list =
tile_polygones[tile_y * tile_size_x + tile_x];
// Пока в списке есть полигоны
while(list.start())
{
// Данные о полигоне
const TPolygonInfo *polygon = list.start()->Object;
// Делаем копию списка вершин полигона
TCircleList<TVertex2D> CellCorners;
CellCorners.copy(polygon->Corners);
// Отсекаем полигон по границам блока
if ( TruncPolygon2D(CellCorners,left)
&& TruncPolygon2D(CellCorners,right)
&& TruncPolygon2D(CellCorners,top)
&& TruncPolygon2D(CellCorners,bottom) )
{
// Растеризуем полигон
RasterPolygonTileZBuffer(
CellCorners, polygon->Color,
tile_min_x, tile_min_y, ZBuffer);
}
// Удаляем полигон из списка
delete list.remove(list.start());
}
}
// Удаляем списки блоков
delete[] tile_polygones;
//Удаляем все спроецированные полигоны
projected_polygones.clear();
}
//-------------------------------------------------------------------------
// Пользовательская тестовая функция
void UserTest()
{
//читаем вариант scene_X, X - номер сцены из задания
if (!LoadScene("Data\\scene_1",5)) return;
// Установить разрешение изображения
SetResolutionFactor(0);
// рисуем сцену
DrawSceneTileZBuffer();
SaveBMP("result.bmp",ImageArray,Colors,RWidth,RHeight);
// выводим изображение на экран
UpdateView(true);
// очищаем данные сцены
ClearScene();
}
//-------------------------------------------------------------------------
// Кол-во вариантов сцены
#define NUMBER_SCENES 5
// Кол-во вариантов разрешения растра
#define NUMBER_RESOLUTIONS 5
//-------------------------------------------------------------------------
// Пользовательская функция
void UserMain()
{
// Пользовательская тестовая функция
// UserTest(); return;
// времена рисования сцены в различных вариантах
double time[NUMBER_SCENES][NUMBER_RESOLUTIONS];
// кол-во пикселов в заданном варианте
long number_pixels[NUMBER_RESOLUTIONS];
// кол-во полигонов в заданном варианте
long number_polygones[NUMBER_SCENES];
for(int i=0;i<NUMBER_SCENES;i++)
{

//читаем вариант scene_X, X - номер сцены из задания
if (!LoadScene("Data\\scene_1",i+1)) return;
number_polygones[i]=NumberFaces;
for(int j=0;j<NUMBER_RESOLUTIONS;j++)
{
// Установить разрешение изображения
SetResolutionFactor(NUMBER_RESOLUTIONS+1-j);
number_pixels[j]=RWidth*RHeight;
double takts=GetTakts();
// рисуем сцену
DrawSceneTileZBuffer();
// определяем время отрисовки сцены
time[i][j]=GetTakts()-takts;
// выводим изображение на экран
UpdateView(true);
}
// изображение последнего варианта сцены сохраняем в файле
if (i==(NUMBER_SCENES-1))
SaveBMP("result.bmp",ImageArray,Colors,RWidth,RHeight);
// очищаем данные сцены
ClearScene();
}
// записываем файл времён
FILE *file = fopen("results.txt","wt");
assert(file,"Ошибка при открытии файла");
if (file)
{
fprintf(file,"Pixels\\Polygones ");
for(int i=0;i<NUMBER_SCENES;i++)
fprintf(file," %5d",number_polygones[i]);
fprintf(file,"\n");
for(int j=0;j<NUMBER_RESOLUTIONS;j++)
{
fprintf(file," %5d ",number_pixels[j]);
for(int i=0;i<NUMBER_SCENES;i++)
{
fprintf(file," %8d",long(time[i][j]/1000));
}
fprintf(file,"\n");
}
}
fclose(file);
}