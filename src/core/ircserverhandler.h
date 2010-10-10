/***************************************************************************
 *   Copyright (C) 2005-10 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef IRCSERVERHANDLER_H
#define IRCSERVERHANDLER_H

#include "corebasichandler.h"
#include "netsplit.h"

class IrcServerHandler : public CoreBasicHandler {
  Q_OBJECT

public:
  IrcServerHandler(CoreNetwork *parent);
  ~IrcServerHandler();

  void handleServerMsg(QByteArray rawMsg);

public slots:
  void handleJoin(const QString &prefix, const QList<QByteArray> &params);
  void handleMode(const QString &prefix, const QList<QByteArray> &params);
  void handleNotice(const QString &prefix, const QList<QByteArray> &params);
  void handlePing(const QString &prefix, const QList<QByteArray> &params);
  void handlePrivmsg(const QString &prefix, const QList<QByteArray> &params);
  void handleQuit(const QString &prefix, const QList<QByteArray> &params);
  void handle324(const QString &prefix, const QList<QByteArray> &params);   // RPL_CHANNELMODEIS

  void defaultHandler(QString cmd, const QString &prefix, const QList<QByteArray> &params);

private slots:
  //! Joins after a netsplit
  /** This slot handles a bulk-join after a netsplit is over
    * \param channel The channel the users joined
    * \param users   The list of users that joind the channel
    * \param modes   The list of modes the users get set
    * \param quitMessage The message we received when the netsplit occured
    */
  void handleNetsplitJoin(const QString &channel, const QStringList &users, const QStringList &modes, const QString &quitMessage);

  //! Quits after a netsplit
  /** This slot handles a bulk-quit after a netsplit occured
    * \param channel The channel the users quitted
    * \param users   The list of users that got split
    * \param quitMessage The message we received when the netsplit occured
    */
  void handleNetsplitQuit(const QString &channel, const QStringList &users, const QString &quitMessage);

  //! Netsplit finished
  /** This slot deletes the netsplit object that sent the finished() signal
    */
  void handleNetsplitFinished();

  void handleEarlyNetsplitJoin(const QString &channel, const QStringList &users, const QStringList &modes);

  //! Destroy any existing netsplits
  /** This slot deletes all netsplit objects
    * Used to get rid of existing netsplits on network reconnect
    */
  void destroyNetsplits();

private:
  void tryNextNick(const QString &errnick, bool erroneus = false);
  bool checkParamCount(const QString &methodName, const QList<QByteArray> &params, int minParams);

  // holds the target for numeric replies or is invalid otherwise
  inline const QString &target() const { return _target; }

  bool _whois;
  QString _target;

  // structure to organize netsplits
  // key: quit message
  // value: the corresponding netsplit object
  QHash<QString, Netsplit*> _netsplits;

#ifdef HAVE_QCA2
  QByteArray decrypt(const QString &target, const QByteArray &message, bool isTopic = false);
#endif
};

#endif
