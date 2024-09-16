-- board.lua

-- The core algorithm behind the Flood Puzzle game 
-- is a variation of the flood fill algorithm, 
-- also known as seed fill or boundary fill. 
-- In the context of this game, it's implemented in 
-- the flood method of the Board class 

local Board = {}
local colors = require 'src.colors'

-- Create a new game board
function Board:new(width, height)
    local board = {
        width = width,
        height = height,
        cells = {},
    }
    -- Set up metatable for object-oriented behavior
    setmetatable(board, self)
    self.__index = self

    -- Initialize the board with random colors
    for y = 1, height do
        board.cells[y] = {}
        for x = 1, width do
            board.cells[y][x] = love.math.random(#colors)
        end
    end

    return board
end

-- Draw the game board
function Board:draw(cellSize, padding)
    for y = 1, self.height do
        for x = 1, self.width do
            -- Get the color for the current cell
            local color = colors[self.cells[y][x]]
            love.graphics.setColor(color)
            -- Draw a rectangle for each cell, applying padding and calculating position
            love.graphics.rectangle('fill', 
                padding + (x-1)*cellSize, 
                padding + (y-1)*cellSize, 
                cellSize, 
                cellSize, 
                0
            )
        end
    end
end

-- Perform flood fill operation on the board
function Board:flood(startX, startY, newColor)
    local oldColor = self.cells[startY][startX]
    -- If the new color is the same as the old color, no need to flood
    if oldColor == newColor then return end

    -- Recursive flood fill function
    local function floodFill(x, y)
        -- Check if we're still within the board boundaries
        if x < 1 or x > self.width or y < 1 or y > self.height then return end
        -- Check if the current cell is not the old color
        if self.cells[y][x] ~= oldColor then return end

        -- Change the color of the current cell
        self.cells[y][x] = newColor
        -- Recursively fill adjacent cells
        floodFill(x+1, y)
        floodFill(x-1, y)
        floodFill(x, y+1)
        floodFill(x, y-1)
    end

    -- Start the flood fill from the given starting point
    floodFill(startX, startY)
end

return Board
