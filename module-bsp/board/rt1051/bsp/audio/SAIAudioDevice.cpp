// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SAIAudioDevice.hpp"

using namespace bsp;

SAIAudioDevice::SAIAudioDevice(AudioDevice::audioCallback_t callback,
                               I2S_Type *base,
                               sai_edma_handle_t *rxHandle,
                               sai_edma_handle_t *txHandle)
    : AudioDevice(callback), _base(base), rx(rxHandle), tx(txHandle)
{}

void SAIAudioDevice::initiateRxTransfer()
{
    audio::Stream::Span dataSpan;

    Source::_stream->reserve(dataSpan);
    auto xfer = sai_transfer_t{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, rx, &xfer);
}

void SAIAudioDevice::initiateTxTransfer()
{
    auto nullSpan = Sink::_stream->getNullSpan();
    auto xfer     = sai_transfer_t{.data = nullSpan.data, .dataSize = nullSpan.dataSize};
    SAI_TransferSendEDMA(_base, tx, &xfer);
}

void SAIAudioDevice::onDataSend()
{
    audio::Stream::Span dataSpan;

    if (!txEnabled || !isSinkConnected()) {
        return;
    }

    /// pop previous read and peek next
    Sink::_stream->consume();
    Sink::_stream->peek(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferSendEDMA(_base, tx, &xfer);
}

void SAIAudioDevice::onDataReceive()
{
    audio::Stream::Span dataSpan;

    if (!rxEnabled || !isSourceConnected()) {
        return;
    }

    /// reserve space for the next read commiting previously reserved block before
    Source::_stream->commit();
    Source::_stream->reserve(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, rx, &xfer);
}

void SAIAudioDevice::enableInput()
{
    if (!isSourceConnected()) {
        LOG_FATAL("No output stream connected!");
        return;
    }

    rxEnabled = true;

    /// initiate first read
    initiateRxTransfer();
}

void SAIAudioDevice::enableOutput()
{
    if (!isSinkConnected()) {
        LOG_FATAL("No input stream connected!");
        return;
    }

    txEnabled = true;

    /// initiate first write
    initiateTxTransfer();
}

void SAIAudioDevice::disableInput()
{
    rxEnabled = false;
}

void SAIAudioDevice::disableOutput()
{
    txEnabled = false;
}