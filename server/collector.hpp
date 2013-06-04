//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_COLLECTOR_HPP
#define HPX_COLLECTOR_HPP

#include <hpx/hpx.hpp>
#include <hpx/runtime/components/component_type.hpp>
#include <hpx/runtime/components/server/abstract_component_base.hpp>
#include <hpx/runtime/components/server/managed_component_base.hpp>

#include <libgeodecomp/misc/region.h>
#include <libgeodecomp/misc/coord.h>
#include <libgeodecomp/io/steerer.h>

#include <serialize/supervector.hpp>

#include <serialize/coord.hpp>
#include <serialize/coordbox.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <grid_storage.hpp>

namespace LibGeoDecomp { namespace HiParSimulator { namespace server {

#if !defined(__ANDROID__) || !defined(ANDROID)
    struct collector_base
        : QObject
    {
        Q_OBJECT

        public:
        public:
    };
#endif

    template <typename CellType>
    struct HPX_COMPONENT_EXPORT collector
      : hpx::components::abstract_managed_component_base<
            collector<CellType>
        >
      , grid_storage
    {
        typedef typename CellType::Topology topology;
        static const int dim = topology::DIMENSIONS;
        
        typedef 
            LibGeoDecomp::SuperVector<CellType>
            buffer_type;

        typedef LibGeoDecomp::Region<dim> RegionType;
        typedef LibGeoDecomp::Coord<dim> CoordType;

        virtual void step_finished() = 0;

        void step_finished_nonvirt()
        {
            this->step_finished();
        }

        HPX_DEFINE_COMPONENT_ACTION_TPL(collector, step_finished_nonvirt, step_finished_action);

        virtual void start(std::string const &, bool) = 0;

        void start_nonvirt(std::string const & name, bool fake_cam)
        {
            this->start(name, fake_cam);
        }

        HPX_DEFINE_COMPONENT_ACTION_TPL(collector, start_nonvirt, start_action);

        virtual boost::shared_ptr<Steerer<CellType> > steer(Region<dim> const & validRegion) = 0;

        boost::shared_ptr<Steerer<CellType> > steer_nonvirt(Region<dim> const & validRegion)
        {
            return this->steer(validRegion);
        }

        HPX_DEFINE_COMPONENT_ACTION_TPL(collector, steer_nonvirt, steer_action);
        
        void new_force_nonvirt(float origin_x, float origin_y, float dest_x, float dest_y)
        {
            return this->new_force(origin_x, origin_y, dest_x, dest_y);
        }
        
        HPX_DEFINE_COMPONENT_ACTION_TPL(collector, new_force_nonvirt, new_force_action);
        
        virtual void new_image(std::vector<signed char> const & data, int width, int height) = 0;
        
        void new_image_nonvirt(std::vector<signed char> const & data, int width, int height)
        {
            return this->new_image(data, width, height);
        }
        
        HPX_DEFINE_COMPONENT_ACTION_TPL(collector, new_image_nonvirt, new_image_action);

    };
}}}

#endif
