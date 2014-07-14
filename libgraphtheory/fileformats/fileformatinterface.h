/*
 *  Copyright 2012-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILEFORMATINTERFACE_H
#define FILEFORMATINTERFACE_H

#include <QObject>
#include "typenames.h"
#include "libgraphtheoryexport.h"

class QUrl;

namespace GraphTheory
{

class FileFormatInterfacePrivate;

/**
 * \class FileFormatInterface
 * This class provides an interface for graph file format plugins.
 * A graph file format plugin must provide implementations of
 * - writeFile(...)
 * - readFile()
 * - extensions()
 * and optionally, if it is not a read and write plugin, an implementation of
 * - pluginCapability()
 * to specify the plugin capabilities.
 */
class GRAPHTHEORY_EXPORT FileFormatInterface : public QObject
{
    Q_OBJECT

public:
    /**
     * Describes the last error of the plugin.
     */
    enum Error {
        None,
        Unknown,
        FileIsReadOnly,
        CouldNotOpenFile,
        NoGraphFound,
        EncodingProblem,
        CouldNotRecognizeFileFormat,
        NotSupportedOperation
    };

    /**
     * Describes the capability of the plugin, i.e., if the plugin can be used
     * to read and/or write files.
     */
    enum PluginType {
        ImportOnly,
        ExportOnly,
        ImportAndExport
    };

    /**
     * Constructor.
     *
     * \param aboutData is description of the plugin
     * \param parent is the object parent
     */
    FileFormatInterface(const QString &componentName, QObject* parent);
    virtual ~FileFormatInterface();

    /**
     * Returns \p PluginType to indicate whether the plugin only provides import, only export
     * or both capabilities. This method should be used to determine in which file dialogs
     * the plugin shall be included.
     *
     * \return is by default PluginType::ImportAndExport
     */
    virtual PluginType pluginCapability() const;

    /**
     * \return true if last read or write operation caused an error.
     */
    bool hasError() const;

    /**
     * Returns last error. If last operation was successful.
     * Use \see hasError() to test if error exists.
     *
     * \return last Error of the plugin.
     */
    Error error() const;

    /**
     * \return last error as human readable text
     */
    QString errorString() const;

    /**
     * File extensions that are common for this file type.
     *
     * \return extension list
     */
    virtual const QStringList extensions() const = 0;

    /**
     * Writes given graph document to formerly specified file \see setFile().
     *
     * \param document is graph document to be serialized
     */
    virtual void writeFile(GraphDocumentPtr document) = 0;

    /**
     * Open given file and imports it into internal format.
     */
    virtual void readFile() = 0;

    /**
     * Set file that shall be used for nexte read or write operation.
     *
     * \param file is QUrl pointing to local file
     */
    void setFile(const QUrl &file);

    /**
     * \return true if a valid graph document was read. Otherwise return false
     */
    virtual bool isGraphDocument() const;

    /**
     * If \see isGraphDocument() returns true, this method returns document created by last
     * \see readFile() call.
     *
     * \return the graph document
     */
    virtual GraphDocumentPtr graphDocument() const;


protected:
    /**
     * \internal
     * Gives current file.
     *
     * \return file that is last used for write/read
     */
    const QUrl & file() const;

    /**
     * \internal
     * Use this function for read plugins to set read graph document
     *
     * \param document that contains the graph
     */
    void setGraphDocument(GraphDocumentPtr document);

    /**
     * \internal
     * Use this function to set or unset the current error of the plugin. To unset an error
     * use Error::None.
     *
     * \param error is type of error, \see FileFormatInterface::Error
     * \param message is optional description for error
     */
    void setError(Error error, const QString &message = QString());

private:
    FileFormatInterfacePrivate * const d;
};
}

#endif
