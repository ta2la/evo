//
// Copyright (C) 2026 Petr Talla. [petr.talla@gmail.com]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//		      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=============================================================================
#include "T2lCmd_selected_2svg.h"

#include "T2lCadLine.h"
#include "T2lCadObject_area.h"
#include "T2lCadObject_linePro.h"
#include "T2lCadObject_text.h"
#include "T2lCadSettings2.h"

#include "T2lGObjectPool.h"
#include "T2lRefColSelection.h"
#include "T2lCmdQueue.h"

#include <TcCmdEngine.h>
#include <TcArgCol.h>

#include <QApplication>
#include <QClipboard>
#include <QString>
#include <QTextStream>
#include <QList>

#include <algorithm>

using namespace T2l;

//===================================================================
Cmd_selected_2svg::Cmd_selected_2svg() : CmdCad("selected to svg")
{
}

//===================================================================
Cmd_selected_2svg::~Cmd_selected_2svg()
{
}

//===================================================================
void Cmd_selected_2svg::registerCmds_()
{
    TcCmdEngine::engine().registerCmd("cmd_selected_2svg",    cmd_activate, "cad");
    TcCmdEngine::engine().registerCmd("cmd_selected_2svg_do", cmd_do,       "cad");
}

//===================================================================
int Cmd_selected_2svg::cmd_activate(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_selected_2svg(), false );
    return 0;
}

//===================================================================
int Cmd_selected_2svg::cmd_do(TcCmdContext* /*context*/, TcArgCol& args)
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if (selected.count() == 0) return args.appendError("no objects selected");

    const double S = CAD_SETTINGS_PARAMS.scale().get();

    QList<CadLine*>           lines;
    QList<CadObject_linePro*> linePros;
    QList<CadObject_area*>    areas;
    QList<CadObject_text*>    texts;
    double minx =  1e18, miny =  1e18, maxx = -1e18, maxy = -1e18;

    auto consume = [&](double x, double y) {
        if (x < minx) minx = x; if (x > maxx) maxx = x;
        if (y < miny) miny = y; if (y > maxy) maxy = y;
    };

    for (long i = 0; i < selected.count(); i++) {
        GObject* obj = selected.get(i)->object();
        if (CadObject_linePro* lp = dynamic_cast<CadObject_linePro*>(obj)) {
            linePros.append(lp);
            for (int j = 0; j < lp->points().count(); j++)
                consume(lp->points().get(j).x(), lp->points().get(j).y());
        }
        else if (CadLine* l = dynamic_cast<CadLine*>(obj)) {
            lines.append(l);
            for (int j = 0; j < l->points().count(); j++)
                consume(l->points().get(j).x(), l->points().get(j).y());
        }
        else if (CadObject_area* a = dynamic_cast<CadObject_area*>(obj)) {
            areas.append(a);
            for (int j = 0; j < a->points().count(); j++)
                consume(a->points().get(j).x(), a->points().get(j).y());
        }
        else if (CadObject_text* t = dynamic_cast<CadObject_text*>(obj)) {
            if (!t->metricText_) continue;  // symbolické texty mají velikost v pixelech → do SVG nepatří
            texts.append(t);
            consume(t->position().x(), t->position().y());
        }
    }

    if (lines.isEmpty() && linePros.isEmpty() && areas.isEmpty() && texts.isEmpty())
        return args.appendError("no lines, areas or metric texts in selection");

    auto colHex = [](const Color& c) {
        return QString("#%1%2%3")
            .arg(c.r(), 2, 16, QChar('0'))
            .arg(c.g(), 2, 16, QChar('0'))
            .arg(c.b(), 2, 16, QChar('0'));
    };

    double w = maxx - minx;
    double h = maxy - miny;
    double pad = std::max(std::max(w, h) * 0.05, 1.0);

    QString body;
    QTextStream out(&body);

    auto writePolyline = [&](auto& pts, const QString& stroke, double width) {
        const double w = (width > 0 ? width : 1.0) * S;
        if (pts.count() == 2) {
            out << "  <line x1='" << pts.get(0).x() << "' y1='" << (-pts.get(0).y())
                << "' x2='" << pts.get(1).x() << "' y2='" << (-pts.get(1).y())
                << "' stroke='" << stroke << "' stroke-width='" << w << "'/>\n";
        } else if (pts.count() > 2) {
            out << "  <polyline points='";
            for (int j = 0; j < pts.count(); j++) {
                if (j) out << " ";
                out << pts.get(j).x() << "," << (-pts.get(j).y());
            }
            out << "' fill='none' stroke='" << stroke << "' stroke-width='" << w << "'/>\n";
        }
    };

    for (CadObject_area* a : areas) {
        auto& pts = a->points();
        if (pts.count() < 2) continue;
        const QString fill   = a->color_use_      ? colHex(a->color_)     : QString("none");
        const QString stroke = a->color_line_use_ ? colHex(a->colorLine_) : QString("none");
        if (pts.count() == 2) {
            double x0 = pts.get(0).x(), y0 = -pts.get(0).y();
            double x1 = pts.get(1).x(), y1 = -pts.get(1).y();
            double x = std::min(x0, x1), y = std::min(y0, y1);
            double w = std::abs(x1 - x0), h = std::abs(y1 - y0);
            out << "  <rect x='" << x << "' y='" << y << "' width='" << w << "' height='" << h
                << "' fill='" << fill << "' stroke='" << stroke << "' stroke-width='" << S << "'/>\n";
        } else {
            out << "  <polygon points='";
            for (int j = 0; j < pts.count(); j++) {
                if (j) out << " ";
                out << pts.get(j).x() << "," << (-pts.get(j).y());
            }
            out << "' fill='" << fill << "' stroke='" << stroke << "' stroke-width='" << S << "'/>\n";
        }
    }

    for (CadLine* l : lines) {
        writePolyline(l->points(), colHex(l->color()), l->width());
    }

    for (CadObject_linePro* lp : linePros) {
        writePolyline(lp->points(), colHex(lp->color()), lp->width());
    }

    auto xmlEscape = [](QString s) {
        return s.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;");
    };

    for (CadObject_text* t : texts) {
        out << "  <text x='" << t->position().x() << "' y='" << (-t->position().y())
            << "' font-size='" << t->size() << "' fill='" << colHex(t->color_) << "'"
            << " font-family='sans-serif'";
        if (t->bold()) out << " font-weight='bold'";
        out << ">" << xmlEscape(t->text()) << "</text>\n";
    }

    QString svg;
    QTextStream svgOut(&svg);
    svgOut << "<svg xmlns='http://www.w3.org/2000/svg' viewBox='"
           << (minx - pad) << " " << (-maxy - pad) << " "
           << (w + 2*pad) << " " << (h + 2*pad) << "'>\n";
    svgOut << body;
    svgOut << "</svg>";

    qApp->clipboard()->setText(svg);

    return 0;
}

//===================================================================
void Cmd_selected_2svg::enterPoint(const Point2F&, Display&)
{
}

//===================================================================
void Cmd_selected_2svg::enterReset(Display&)
{
}

//===================================================================
QString Cmd_selected_2svg::dialogTml() const
{
    QString result;

    result += "TC;CT;text: scale:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(CAD_SETTINGS_PARAMS.scale().get()) + ";";
    result += "cmd: cad_set_scale \"$TEXT\";";
    result += "width: 80;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CB;text: Export to clipboard;cmd: cmd_selected_2svg_do;;";

    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_selected_2svg::hint() const
{
    return "set scale and click Export to copy SVG to clipboard";
}
