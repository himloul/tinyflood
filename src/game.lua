local Game = {}
local Board = require 'src.board'
local colors = require 'src.colors'


-- At the top of game.lua, add:
local config = require 'src.config'

-- Create a new game instance
function Game:new()
    local game = {
        board = Board:new(config.BOARD_WIDTH, config.BOARD_HEIGHT),  -- Use config values
        cellSize = 30,              -- Size of each cell in pixels
        moves = 0,                  -- Current number of moves
        maxMoves = 15,              -- Maximum allowed moves
        gameOver = false,           -- Flag to track if the game is over
        win = false,                -- Flag to track if the player has won
        
        padding = 40,               -- Padding around the game board
        buttonSpacing = 10,         -- Spacing between color buttons

        restartButtonX = 0,  -- Will be set in draw method
        restartButtonY = 0,  -- Will be set in draw method
        restartButtonWidth = 100,
        restartButtonHeight = 30,
        textColor = colors[1],
        buttonColor = {0.9, 0.9, 0.9},
        bgColor = {1, 1, 1},

        font = love.graphics.newFont("assets/fonts/monogram.ttf", 25),  -- Original font
        titleFont = love.graphics.newFont("assets/fonts/monogram.ttf", 50),  -- Larger font for title

        titleImage = love.graphics.newImage("assets/images/tinyflood.png"),

        gameOverSound = love.audio.newSource("assets/sounds/hurt.wav", "static"),
        winSound = love.audio.newSource("assets/sounds/win.wav", "static"),

    }
    setmetatable(game, self)
    self.__index = self

    -- Load a custom font
    -- The font file should be located in the 'fonts' folder
    -- Adjust the size (16) as needed for your game
    -- Load the custom font

    return game
end

-- Update game state (can be used for animations or time-based events)
function Game:update(dt)
    if not self.gameOver and not self.win then
        if self.moves >= self.maxMoves then
            self.gameOver = true
            love.audio.play(self.gameOverSound)
        elseif self:checkWin() then
            self.win = true
            love.audio.play(self.winSound)
        end
    end
    -- Add any other game state updates here
end

-- Draw the game
function Game:draw()
    -- Set background color
    love.graphics.setBackgroundColor(self.bgColor)
    
    -- Draw the game board
    self.board:draw(self.cellSize, self.padding)

    -- Set the custom font for text drawing
    love.graphics.setFont(self.font)
    
    -- Draw the move counter
    love.graphics.setColor(self.textColor)
    love.graphics.print("Moves: " .. self.moves .. "/" .. self.maxMoves, self.padding, self.padding*0.2)
    
    -- Calculate the Y position for the color buttons
    local buttonY = self.padding + self.board.height * self.cellSize + self.buttonSpacing

    -- Draw color selection buttons
    for i, color in ipairs(colors) do
        love.graphics.setColor(color)
        love.graphics.rectangle('fill', 
            self.padding + (i-1)*30, 
            buttonY, 
            25, 
            25,
            3 -- Corner radius
        )
    end

    -- Draw restart button below color palette
    self.restartButtonY = buttonY + 25 + self.buttonSpacing
    self.restartButtonX = self.padding
    self:drawRestartButton()

    -- Draw win or game over messages in a black ribbon
    if self:checkWin() or self.moves >= self.maxMoves then
        self:drawMessageRibbon()
    end

    -- Draw the custom cursor last so it appears on top of everything
    self:drawCustomCursor()
end

-- Handle mouse press events
function Game:mousepressed(x, y, button)
    if button == 1 then  -- Left mouse button
        -- Check if restart button was clicked
        if x >= self.restartButtonX and x <= self.restartButtonX + self.restartButtonWidth and
           y >= self.restartButtonY and y <= self.restartButtonY + self.restartButtonHeight then
            self:restartGame()
            return
        end

        -- If game is over or won, only allow restart button interaction
        if self.gameOver or self.win then
            return
        end

        -- Calculate the position of color buttons
        local buttonY = self.padding + self.board.height * self.cellSize + self.buttonSpacing
        local colorIndex = math.floor((x - self.padding) / 30) + 1
    
        -- Check if a color button was clicked
        if colorIndex >= 1 and colorIndex <= #colors and 
           y > buttonY and 
           y < buttonY + 25 then
            -- Only flood fill and increment moves if the selected color is different
            if self.board.cells[1][1] ~= colorIndex then
                self.board:flood(1, 1, colorIndex)
                self.moves = self.moves + 1
                
                -- Check for win or game over conditions
                if self:checkWin() then
                    self.win = true
                    love.audio.play(self.winSound)
                    print("You win!")
                elseif self.moves >= self.maxMoves then
                    self.gameOver = true
                    love.audio.play(self.gameOverSound)
                    print("Game over!")
                end
            end
        end
    end
end

function Game:keypressed(key)
    if key == 'escape' then
        return "menu"  -- This will signal to return to the menu
    end
    return nil  -- Return nil for any other key
end

-- Check if the player has won
function Game:checkWin()
local firstColor = self.board.cells[1][1]
for y = 1, self.board.height do
    for x = 1, self.board.width do
        if self.board.cells[y][x] ~= firstColor then
            return false
        end
    end
end
return true
end

-- Method to draw Restart button
function Game:drawRestartButton()
    love.graphics.setColor(self.buttonColor)  -- Light gray color
    love.graphics.rectangle('fill', self.restartButtonX, self.restartButtonY, self.restartButtonWidth, self.restartButtonHeight, 5) -- Corner radius
    love.graphics.setColor(colors[1])  -- Black color for text
    love.graphics.printf("Restart", self.restartButtonX, self.restartButtonY + 5, self.restartButtonWidth, "center")
end

-- Method to restart the game
function Game:restartGame()
    self.board = Board:new(config.BOARD_WIDTH, config.BOARD_HEIGHT)  -- Create a new board
    self.moves = 0
    self.gameOver = false
    self.win = false
    love.audio.stop(self.gameOverSound)  -- Stop the game over sound
    love.audio.stop(self.winSound)       -- Stop the win sound
    -- Any other state you need to reset
end

-- Menu screen

function Game:drawMenu()
    love.graphics.setBackgroundColor(self.bgColor)

    -- Draw the title
    love.graphics.setFont(self.titleFont)
    love.graphics.setColor(colors[2])
    love.graphics.printf(config.GAME_TITLE, 0, 100, love.graphics.getWidth(), "center")

    -- Draw the title image
    local imageWidth = self.titleImage:getWidth()
    local imageHeight = self.titleImage:getHeight()
    local scale = 0.5  -- Adjust this value to resize the image if needed
    love.graphics.setColor(1, 1, 1)  -- White color to draw the image without tint
    love.graphics.draw(self.titleImage, 
        (love.graphics.getWidth() - imageWidth * scale) / 2, 
        200,  -- Adjust this value to position the image vertically
        0, 
        scale, 
        scale
    )
    
    -- Draw menu options
    love.graphics.setFont(self.font)
    love.graphics.setColor(colors[1])
    love.graphics.printf("Start", 0, 300, love.graphics.getWidth(), "center")
    love.graphics.printf("Instructions", 0, 350, love.graphics.getWidth(), "center")
    -- Draw the custom cursor last so it appears on top of everything
    self:drawCustomCursor()
end

function Game:drawInstructions()
    love.graphics.setBackgroundColor(self.bgColor)
    love.graphics.setFont(self.font)

    local padding = 40
    local screenWidth = love.graphics.getWidth()

    -- Draw title centered
    love.graphics.setColor(colors[2])
    love.graphics.printf("Instructions", padding, 100, screenWidth - padding * 2, "left")

    -- Draw instructions aligned left with padding
    love.graphics.setColor(self.textColor)
    love.graphics.printf("Click colors to flood the board, starting from the top-left corner.", padding, 150, screenWidth - padding * 2, "left")
    love.graphics.printf("Try to fill the entire board with one color in as few moves as possible.", padding, 200, screenWidth - padding * 2, "left")

    -- Draw 'Back' button centered at the bottom
    love.graphics.printf("Back", 0, love.graphics.getHeight() - 50, screenWidth, "center")

    -- Draw the custom cursor last so it appears on top of everything
    self:drawCustomCursor()
end

function Game:handleMenuClick(x, y)
    if y > 300 and y < 330 then
        return "playing"
    elseif y > 350 and y < 380 then
        return "instructions"
    end
    return "menu"
end

function Game:handleInstructionsClick(x, y)
    local screenWidth = love.graphics.getWidth()
    local screenHeight = love.graphics.getHeight()
    
    -- Check if the click is within the "Back" button area
    if y > screenHeight - 70 and y < screenHeight - 30 then
        if x > screenWidth / 2 - 50 and x < screenWidth / 2 + 50 then
            return "menu"
        end
    end
    return "instructions"
end

function Game:drawCustomCursor()
    local mouseX, mouseY = love.mouse.getPosition()
    love.graphics.setColor(1, 1, 1)  -- White color
    love.graphics.circle("fill", mouseX, mouseY, 5)  -- 5 is the radius of the circle
    love.graphics.setColor(0, 0, 0)  -- Black color for outline
    love.graphics.circle("line", mouseX, mouseY, 5)
end

function Game:drawMessageRibbon()
    local ribbonHeight = 60
    local screenWidth = love.graphics.getWidth()
    local screenHeight = love.graphics.getHeight()
    local ribbonY = (screenHeight - ribbonHeight) / 2

    -- Draw semi-transparent layer over the entire screen
    love.graphics.setColor(1, 1, 1, 0.5)  -- White with 50% opacity
    love.graphics.rectangle('fill', 0, 0, screenWidth, screenHeight)

    -- Determine ribbon color and message
    local isWin = self:checkWin()
    local ribbonColor, message
    if isWin then
        ribbonColor = {0, 0.5, 0}  -- Green for win
        message = "You win!"
    else
        ribbonColor = {0.5, 0, 0}  -- Red for game over
        message = "Game over!"
    end

    -- Draw colored ribbon
    love.graphics.setColor(ribbonColor)
    love.graphics.rectangle('fill', 0, ribbonY, screenWidth, ribbonHeight)

    -- Draw message
    love.graphics.setColor(1, 1, 1)  -- White text
    love.graphics.printf(message, 0, ribbonY + (ribbonHeight - self.font:getHeight()) / 2, screenWidth, "center")
end

-- Return Game

return Game

