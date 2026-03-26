/*
/
// filename: Grid.h
// author: Callen Betts
// brief: defines a grid data structure to handle entity comparisons
/
*/

/* desc:
* The world is split up into a grid. Each grid cell has its own entity lists of colliders and entities.
* Each scene has its own container of grid cells, with a maximum amount cells depending on the world size.
* This means far less comparisons; however, we cannot have objects larger than the grid size
*/

#pragma once

namespace World
{

  class Grid
  {

  public:


  private:


  };
}