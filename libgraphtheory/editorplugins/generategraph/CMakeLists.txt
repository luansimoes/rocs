# Copyright 2012-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Boost requires exceptions for this plugin
kde_enable_exceptions()

set(generategraph_SRCS
    generategraphplugin.cpp
    generategraphwidget.cpp
    ../../logging.cpp
)

#boost requires exceptions
kde_source_files_enable_exceptions(generategraphplugin.cpp)

ki18n_wrap_ui(generategraph_SRCS generategraphwidget.ui)
add_library(generategraphplugin
    MODULE
    ${generategraph_SRCS}
)

target_link_libraries(generategraphplugin
    PUBLIC
    rocsgraphtheory
    KF5::Completion
)

ecm_optional_add_subdirectory(autotests)

install(TARGETS generategraphplugin DESTINATION ${PLUGIN_INSTALL_DIR}/rocs/editorplugins)
