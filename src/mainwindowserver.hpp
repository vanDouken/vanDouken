//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_MAINWINDOWSERVER_HPP
#define VANDOUKEN_MAINWINDOWSERVER_HPP

#if !defined(__MIC)

#include <hpx/hpx.hpp>
#include <hpx/include/components.hpp>

#include <boost/serialization/split_free.hpp>

#include <QImage>
#include <QBuffer>
#include <QByteArray>

namespace vandouken
{
    class MainWindow;

    struct MainWindowServer
      : public hpx::components::managed_component_base<MainWindowServer>
    {
    public:
        MainWindowServer()
        {
            BOOST_ASSERT(false);
        }
        typedef 
            hpx::components::server::create_component_action1<
                vandouken::MainWindowServer
              , vandouken::MainWindow *
            >
            CreateAction;

        typedef 
            hpx::components::server::create_component_action1<
                vandouken::MainWindowServer
              , vandouken::MainWindow * const
            >
            ConstCreateAction;

        MainWindowServer(MainWindow *mainWindow) :
            mainWindow(mainWindow)
        {}

        void stateChanged(int, bool);
        HPX_DEFINE_COMPONENT_ACTION(MainWindowServer, stateChanged, StateChangedAction);

        QImage getImage();
        HPX_DEFINE_COMPONENT_ACTION(MainWindowServer, getImage, GetImageAction);

        MainWindow *mainWindow;
    };
}

template <typename ARCHIVE>
void load(ARCHIVE& ar, QImage & image, unsigned)
{
    int size = 0;
    ar & size;
    if(size > 0)
    {
        QByteArray ba(size, 0);
        ar & boost::serialization::make_array(ba.data(), size);
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadOnly);
        image.load(&buffer, "PNG");
    }
}

template <typename ARCHIVE>
void save(ARCHIVE& ar, QImage const & image, unsigned)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    int size = ba.size();
    ar & size;
    if(size > 0)
    {
        ar & boost::serialization::make_array(ba.data(), ba.size());
    }
}

BOOST_SERIALIZATION_SPLIT_FREE(QImage)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(QImage, QImageLco)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::MainWindowServer::CreateAction
  , vandoukenMainWindowServerCreateAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::MainWindowServer::ConstCreateAction
  , vandoukenMainWindowServerConstCreateAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::MainWindowServer::StateChangedAction,
    MainWindowServerStateChangedAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::MainWindowServer::GetImageAction,
    MainWindowGetImageAction)

#endif
#endif
