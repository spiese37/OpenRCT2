/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Entity.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

struct haunted_house_bound_box
{
    int16_t offset_x;
    int16_t offset_y;
    int16_t length_x;
    int16_t length_y;
};

/** rct2: 0x1428180 */
static haunted_house_bound_box haunted_house_data[] = {
    { 6, 0, 42, 24 }, { 0, 0, 0, 0 }, { -16, -16, 32, 32 }, { 0, 0, 0, 0 }, { 0, 6, 24, 42 }, { 0, 0, 0, 0 },
};

/**
 * rct2: 0x0076F72C
 */
static void paint_haunted_house_structure(
    paint_session* session, const Ride* ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint8_t part, uint16_t height)
{
    const TileElement* savedTileElement = static_cast<const TileElement*>(session->CurrentlyDrawnItem);

    uint8_t frameNum = 0;

    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    uint32_t baseImageId = rideEntry->vehicles[0].base_image_id;

    auto vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session->InteractionType = ViewportInteractionItem::Entity;
        session->CurrentlyDrawnItem = vehicle;
        frameNum = vehicle->Pitch;
    }

    uint32_t imageId = (baseImageId + direction) | session->TrackColours[SCHEME_MISC];
    haunted_house_bound_box boundBox = haunted_house_data[part];
    PaintAddImageAsParent(
        session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
        boundBox.offset_y, height);

    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level <= 0 && frameNum != 0)
    {
        switch (direction)
        {
            case 0:
                imageId = baseImageId + 3 + frameNum;
                break;
            case 1:
                imageId = baseImageId + 21 + frameNum;
                break;
            case 2:
                imageId = baseImageId + 39 + frameNum;
                break;
            case 3:
                imageId = baseImageId + 57 + frameNum;
                break;
        }
        imageId = imageId | session->TrackColours[SCHEME_MISC];
        PaintAddImageAsChild(
            session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
            boundBox.offset_y, height);
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType = ViewportInteractionItem::Ride;
}

/**
 * rct2: 0x0076E9B0
 */
static void paint_haunted_house(
    paint_session* session, const Ride* ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    StationObject* stationObject = nullptr;
    if (ride != nullptr)
        stationObject = ride_get_station_object(ride);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    if (ride != nullptr)
    {
        track_paint_util_paint_fences(
            session, edges, session->MapPosition, trackElement, ride, session->TrackColours[SCHEME_MISC], height,
            fenceSpritesRope, session->CurrentRotation);
    }

    switch (trackSequence)
    {
        case 3:
            paint_haunted_house_structure(session, ride, direction, 32, -32, 0, height + 3);
            break;
        case 6:
            paint_haunted_house_structure(session, ride, direction, -32, 32, 4, height + 3);
            break;
        case 7:
            paint_haunted_house_structure(session, ride, direction, -32, -32, 2, height + 3);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
            break;
        case 3:
            // right
            cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
            break;
        case 6:
            // left
            cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
            break;
        case 7:
            // bottom
            cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
            break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

/**
 * rct2: 0x0076E7B0
 */
TRACK_PAINT_FUNCTION get_track_paint_function_haunted_house(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return paint_haunted_house;
}
