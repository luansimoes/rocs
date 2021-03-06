/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef METHOD_H
#define METHOD_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QVariantList>

class Parameter;

class Method : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QVariantList description READ description)
    Q_PROPERTY(QVariant parameters READ parametersVar)
    Q_PROPERTY(QString returnType READ returnType)
    Q_PROPERTY(QString returnTypeLink READ returnTypeLink)
    Q_PROPERTY(QString documentAnchor READ apiDocumentAnchor)

public:
    explicit Method(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);
    QVariantList description() const;
    void setDescription(const QStringList &description);
    QString returnType() const;
    void setReturnType(const QString &type);
    QVariant parametersVar() const;
    QList<Parameter*> parameters() const;
    void addParameter(const QString &name, const QString &type, const QString &info, const QString &typeLink);
    QString apiDocumentAnchor();
    QString returnTypeLink() const;
    void setReturnTypeLink(const QString &link);

private:
    Q_DISABLE_COPY(Method)
    QString m_name;
    QStringList m_description;
    QString m_returnType;
    QString m_returnTypeLink;
    QList<Parameter*> m_parameters;
};

// Q_DECLARE_METATYPE(QList<ParameterDocumentation*>);
// Q_DECLARE_METATYPE(ParameterDocumentation*);

#endif
