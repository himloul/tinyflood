-- main.lua
-- This is the entry point for the LÖVE framework

-- Import the Game module
local Game = require 'src.game'
local gameState = "menu"
local config = require 'src.config'

-- love.load is called once at the start of the game
function love.load()
    -- achieve a pixel-perfect look
    love.graphics.setDefaultFilter("nearest", "nearest")
    -- icon
    local icon = love.image.newImageData("assets/images/tinyflood.png")
    love.window.setIcon(icon)

    -- Create a new instance of the Game
    love.window.setMode(30*12+40*2, 30*12+40*2+30+40, {resizable=false, vsync=true})
    love.mouse.setVisible(false)  -- Hide the default cursor
    love.window.setTitle(config.GAME_TITLE)
    game = Game:new()

end

-- love.update is called every frame, used for game logic
-- 'dt' is the time elapsed since the last update
function love.update(dt)
    -- Update the game state
    game:update(dt)
end

-- love.draw is called every frame, used for rendering

function love.draw()
    if gameState == "menu" then
        game:drawMenu()
    elseif gameState == "playing" then
        game:draw()
    elseif gameState == "instructions" then
        game:drawInstructions()
    end
end

function love.mousepressed(x, y, button)
    if gameState == "menu" then
        gameState = game:handleMenuClick(x, y)
    elseif gameState == "playing" then
        game:mousepressed(x, y, button)
    elseif gameState == "instructions" then
        gameState = game:handleInstructionsClick(x, y)
    end
end

function love.keypressed(key)
    if gameState == "playing" or gameState == "instructions" then
        local newState = game:keypressed(key)
        if newState then
            gameState = newState
        end
    end
end
