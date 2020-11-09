// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerDesktop.hpp"

#include <map>    // for map
#include <vector> // for vector

#include "MessageHandler.hpp"      // for MessageHandler, MessageHandler::sendQueue
#include "ParserFSM.hpp"           // for StateMachine
#include "bsp/usb/usb.hpp"         // for usbCDCInit, usbCDCSend
#include "log/log.hpp"             // for LOG_ERROR, LOG_INFO
#include "projdefs.h"              // for pdTRUE
#include "queue.h"                 // for xQueueReceive, QueueDefinition, QueueHandle_t

bool WorkerDesktop::handleMessage(uint32_t queueID)
{

    QueueHandle_t queue = queues[queueID];

    std::string qname = queueNameMap[queue];
    LOG_INFO("[ServiceDesktop:Worker] Received data from queue: %s", qname.c_str());

    static std::string receiveMsg;
    static std::string *sendMsg;

    if (qname == SERVICE_QUEUE_NAME) {
        LOG_ERROR("[ServiceDesktop:Worker] Service Queue invoked but not implemented!");
    }

    if (qname == RECEIVE_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &receiveMsg, 0) != pdTRUE)
            return false;

        parser.processMessage(receiveMsg);
    }

    // TODO: Consider moving sendBuffer receive to bsp driver
    if (qname == SEND_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &sendMsg, 0) != pdTRUE)
            return false;

        bsp::usbCDCSend(sendMsg);
    }

    return true;
}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);

    if ((bsp::usbInit(Worker::getQueueByName(WorkerDesktop::RECEIVE_QUEUE_BUFFER_NAME)) == nullptr)) {
        LOG_ERROR("won't start desktop service without serial port");
        return false;
    }

    parserFSM::MessageHandler::sendQueue = Worker::getQueueByName(WorkerDesktop::SEND_QUEUE_BUFFER_NAME);

    return true;
}

bool WorkerDesktop::deinit(void)
{
    Worker::deinit();
    return true;
}
