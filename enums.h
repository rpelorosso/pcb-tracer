#ifndef ENUMS_H
#define ENUMS_H

#include <string>

enum class LinkSide {
	FRONT,
	BACK,
	WIP,
	NOTES,
	NODE,
	HIGHLIGHTED,
};

enum class DrawingState {
	IDLE,
	TRACKS,
	IC,
	NOTES
};

enum class Color {
	FRONT,
	BACK,
	WIP,
	NOTES,
	NODE,
	HIGHLIGHTED,
};

namespace ColorUtils {
    inline QString toString(Color color) {
        switch(color) {
            case Color::FRONT: return "front";
            case Color::BACK: return "back";
            case Color::WIP: return "work_in_progress";
            case Color::NOTES: return "notes";
            case Color::NODE: return "node";
            case Color::HIGHLIGHTED: return "highlighted";
            default: return "unknown";
        }
    }

    inline Color fromString(const QString& str) {
        if (str == "front") return Color::FRONT;
        if (str == "back") return Color::BACK;
        if (str == "work_in_progress") return Color::WIP;
        if (str == "notes") return Color::NOTES;
        if (str == "node") return Color::NODE;
        if (str == "highlighted") return Color::HIGHLIGHTED;
        qDebug() << "Warning: LinkSideUtils::fromString received unknown string: " << str;
        return Color::FRONT;
    }       


    inline Color fromLinkSide(LinkSide side) {
        switch(side) {
            case LinkSide::FRONT: return Color::FRONT;
            case LinkSide::BACK: return Color::BACK;
            case LinkSide::WIP: return Color::WIP;
            case LinkSide::NOTES: return Color::NOTES;
            case LinkSide::NODE: return Color::NODE;
            case LinkSide::HIGHLIGHTED: return Color::HIGHLIGHTED;
            default: return Color::FRONT;
        }
    }    
}

namespace LinkSideUtils {
    inline QString toString(LinkSide side) {
        switch(side) {
            case LinkSide::FRONT: return "front";
            case LinkSide::BACK: return "back";
            case LinkSide::WIP: return "work_in_progress";
            case LinkSide::NOTES: return "notes";
            case LinkSide::NODE: return "node";
            case LinkSide::HIGHLIGHTED: return "highlighted";
            default: return "unknown";
        }
    }


    inline LinkSide fromString(const QString& str) {
        if (str == "front") return LinkSide::FRONT;
        if (str == "back") return LinkSide::BACK;
        if (str == "work_in_progress") return LinkSide::WIP;
        if (str == "notes") return LinkSide::NOTES;
        if (str == "node") return LinkSide::NODE;
        if (str == "highlighted") return LinkSide::HIGHLIGHTED;
        qDebug() << "Warning: LinkSideUtils::fromString received unknown string: " << str;
        return LinkSide::FRONT;
    }    
}

#endif // ENUMS_H


