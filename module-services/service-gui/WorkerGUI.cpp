﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerGUI.hpp"

#include <DrawCommand.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <projdefs.h>
#include <queue.h>
#include <Renderer.hpp>
#include <semphr.h>
#include <Service/Bus.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <service-gui/ServiceGUI.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace sgui
{

    WorkerGUI::WorkerGUI(ServiceGUI *service) : Worker(service)
    {}

    bool WorkerGUI::handleMessage(uint32_t queueID)
    {
        QueueHandle_t queue = queues[queueID];

        sgui::ServiceGUI *serviceGUI = reinterpret_cast<sgui::ServiceGUI *>(service);

        // queue for receiving rendering commands
        if (queueID == 0) {
            //		LOG_INFO("Received rendering commands");

            sys::WorkerCommand received;
            xQueueReceive(queue, &received, 0);

            // take all unique pointers
            std::list<std::unique_ptr<gui::DrawCommand>> uniqueCommands;

            if (xSemaphoreTake(serviceGUI->semCommands, pdMS_TO_TICKS(1000)) == pdTRUE) {
                uniqueCommands = std::move(serviceGUI->commands);
                xSemaphoreGive(serviceGUI->semCommands);
            }
            else {
                LOG_ERROR("Failed to acquire semaphore");
            }

            //		uint32_t start_tick = xTaskGetTickCount();
            serviceGUI->renderer.render(serviceGUI->renderContext, uniqueCommands);
            //		uint32_t end_tick = xTaskGetTickCount();
            //		LOG_INFO("[WorkerGUI] RenderingTime: %d", end_tick - start_tick);


            // notify gui service that rendering is complete
            auto message = std::make_shared<sys::DataMessage>(MessageType::GUIRenderingFinished);
            sys::Bus::SendUnicast(message, this->service->GetName(), this->service);
        }
        return true;
    }

} /* namespace sgui */
