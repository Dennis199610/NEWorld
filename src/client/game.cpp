/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "game.h"
#include "renderer.h"
#include "utils.h"
#include <logger.h>
#include "network.h"
#include <boost/dll/shared_library.hpp>
#include <jsonhelper.h>
#include "window.h"

Game::Game(PluginManager& pm, const BlockManager& bm)
    : m_blocks(bm), m_plugins(pm), m_world("TestWorld", pm, bm), m_player(&m_world) // TODO: read from settings
{
    // TODO: start the server only when it's a single player mode.

    m_localServerThread = std::thread([]
    {
        std::string file = getJsonValue<std::string>(getSettings()["server"]["file"], "nwserver").c_str();
        const char *argv[] = { file.c_str(),"-single-player-mode" };
        try {
            Library(file).get<void NWAPICALL(int, char**)>("main")(sizeof(argv) / sizeof(argv[0]), const_cast<char**>(argv));
        }
        catch (boost::system::system_error exception)
        {
            errorstream << "Failed to run the server! Error: " << exception.what();
        }
    });


    mConn.connect("127.0.0.1",9887);// TODO: get address and port from settingsmanager. --Miigon

    mConn.send(c2s::CreateLoginDirect(mFbb, "test", "123456", NEWorldVersion),
               PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

    // TEMP CODE
    // Load some chunks at client side to test rendering
    m_world.setRenderDistance(4);
    m_player.setPosition(Vec3d(-16.0, 48.0, 32.0));
    m_player.setRotation(Vec3d(-45.0, -22.5, 0.0));
    Vec3i::for_range(-6, 6, [&](const Vec3i& pos)
    {
        m_world.addChunk(pos);
    });
    // END TEMP CODE

    // Initialize rendering
    m_texture = Texture::loadTextureRGBA("./res/test.png");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}


void Game::update()
{
    m_player.update();
    m_world.renderUpdate(Vec3i(m_player.getPosition()));
    m_world.update();

    // TODO: Read keys from the configuration file
    if (Window::isKeyDown(SDL_SCANCODE_UP))
        m_player.rotate(Vec3d(1.5, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_DOWN))
        m_player.rotate(Vec3d(-1.5, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_RIGHT))
        m_player.rotate(Vec3d(0.0, -1.5, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_LEFT))
        m_player.rotate(Vec3d(0.0, 1.5, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_W))
        m_player.accelerate(Vec3d(0.0, 0.0, -0.03));
    if (Window::isKeyDown(SDL_SCANCODE_S))
        m_player.accelerate(Vec3d(0.0, 0.0, 0.03));
    if (Window::isKeyDown(SDL_SCANCODE_A))
        m_player.accelerate(Vec3d(-0.03, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_D))
        m_player.accelerate(Vec3d(0.03, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_SPACE))
        m_player.accelerate(Vec3d(0.0, 0.05, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_LCTRL) || Window::isKeyDown(SDL_SCANCODE_RCTRL))
        m_player.accelerate(Vec3d(0.0, -0.05, 0.0));
}

void Game::render() const
{
    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    m_texture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, float(windowWidth) / windowHeight, 0.1f, 300.0f);
    Renderer::restoreScale();
    Renderer::rotate(-m_player.getRotation().x, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().y, Vec3f(0.0f, 1.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().z, Vec3f(0.0f, 0.0f, 1.0f));
    Renderer::translate(-m_player.getPosition());

    // Render
    m_world.render(Vec3i(m_player.getPosition()));

    glDisable(GL_TEXTURE_2D);

    // TEMP CODE
    // To show the world coordinates
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    /*
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 32.0f);
    glVertex3f(32.0f, 0.0f, 32.0f);
    glVertex3f(32.0f, 0.0f, 0.0f);
    glEnd();
    */
    // X
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-256.0f, 0.0f, 0.0f);
    glVertex3f(256.0f, 0.0f, 0.0f);
    glEnd();
    // Y
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -256.0f, 0.0f);
    glVertex3f(0.0f, 256.0f, 0.0f);
    glEnd();
    // Z
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -256.0f);
    glVertex3f(0.0f, 0.0f, 256.0f);
    glEnd();
    glEnable(GL_CULL_FACE);
    // END TEMP CODE

    glDisable(GL_DEPTH_TEST);
}