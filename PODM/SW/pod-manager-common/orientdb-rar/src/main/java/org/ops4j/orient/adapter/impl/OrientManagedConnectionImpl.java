/*
 * Copyright (c) 2015 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright 2013 Harald Wellmann
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.ops4j.orient.adapter.impl;

import com.orientechnologies.orient.core.db.ODatabaseInternal;
import com.orientechnologies.orient.core.db.document.ODatabaseDocumentTx;
import com.orientechnologies.orient.object.db.OObjectDatabaseTx;
import com.tinkerpop.blueprints.impls.orient.OrientBaseGraph;
import com.tinkerpop.blueprints.impls.orient.OrientGraph;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.resource.ResourceException;
import javax.resource.spi.ConnectionEvent;
import javax.resource.spi.ConnectionEventListener;
import javax.resource.spi.ConnectionRequestInfo;
import javax.resource.spi.LocalTransaction;
import javax.resource.spi.ManagedConnection;
import javax.resource.spi.ManagedConnectionMetaData;
import javax.security.auth.Subject;
import javax.transaction.xa.XAResource;
import java.io.Closeable;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import static javax.resource.spi.ConnectionEvent.CONNECTION_CLOSED;
import static javax.resource.spi.ConnectionEvent.CONNECTION_ERROR_OCCURRED;
import static javax.resource.spi.ConnectionEvent.LOCAL_TRANSACTION_COMMITTED;
import static javax.resource.spi.ConnectionEvent.LOCAL_TRANSACTION_ROLLEDBACK;
import static javax.resource.spi.ConnectionEvent.LOCAL_TRANSACTION_STARTED;
/**
 * @author Harald Wellmann
 *
 */
public class OrientManagedConnectionImpl implements ManagedConnection, Closeable {
    private static Logger log = LoggerFactory.getLogger(OrientManagedConnectionImpl.class);
    private OrientManagedConnectionFactoryImpl mcf;
    private ODatabaseInternal<?> db;
    private PrintWriter logWriter;
    private final List<ConnectionEventListener> listeners = new ArrayList<>();
    private ConnectionRequestInfo cri;
    private OrientDatabaseConnectionImpl connection;
    private String engine;
    class OrientLocalTransaction implements LocalTransaction {
        @Override
        public void begin() throws ResourceException {
            db.activateOnCurrentThread();
            log.debug("begin()");
            openDatabase();
            db.getLocalCache().invalidate();
            db.begin();
            fireConnectionEvent(LOCAL_TRANSACTION_STARTED);
        }
        @Override
        public void commit() throws ResourceException {
            log.debug("commit()");
            OrientBaseGraph activeGraph = OrientGraph.getActiveGraph();
            if (activeGraph != null) {
                activeGraph.commit();
                db.commit();
                activeGraph.shutdown(false);
            } else {
                db.commit();
            }
            connection.close();
            fireConnectionEvent(LOCAL_TRANSACTION_COMMITTED);
        }
        @Override
        public void rollback() throws ResourceException {
            log.debug("rollback()");
            OrientBaseGraph activeGraph = OrientGraph.getActiveGraph();
            if (activeGraph != null) {
                activeGraph.rollback();
                db.rollback();
                activeGraph.shutdown(false);
            } else {
                db.rollback();
            }
            connection.close();
            fireConnectionEvent(LOCAL_TRANSACTION_ROLLEDBACK);
        }
    }
    public OrientManagedConnectionImpl(OrientManagedConnectionFactoryImpl mcf, ConnectionRequestInfo cri) throws ResourceException {
        this.mcf = mcf;
        this.cri = cri;
        determineEngine();
        createDatabaseHandle();
        createDatabaseIfNeeded();
    }
    @Override
    public Object getConnection(Subject subject, ConnectionRequestInfo cxRequestInfo) throws ResourceException {
        log.debug("getConnection()");
        connection = new OrientDatabaseConnectionImpl(db, this);
        if (db.isClosed()) {
            openDatabase();
        }
        return connection;
    }
    private void createDatabaseHandle() {
        String type = mcf.getType();
        String url = mcf.getConnectionUrl();
        log.debug("instantiating Orient Database of type [{}] with URL [{}]", type, url);
        switch (type) {
            case "document":
            case "graph":
                this.db = new ODatabaseDocumentTx(url);
                break;
            case "object":
                this.db = new OObjectDatabaseTx(url);
                break;
        }
    }
    private synchronized void createDatabaseIfNeeded() {
        if (!engine.equals("remote")) {
            if (!db.exists()) {
                db.create();
            }
        }
    }
    private synchronized void determineEngine() throws ResourceException {
        int colon = mcf.getConnectionUrl().indexOf(':');
        if (colon == -1) {
            throw new ResourceException();
        }
        this.engine = mcf.getConnectionUrl().substring(0, colon);
    }
    private void openDatabase() {
        if (db.isClosed()) {
            log.trace("opening database for user [{}]", mcf.getUsername());
            db.open(mcf.getUsername(), mcf.getPassword());
        }
    }
    private void closeDatabase() {
        db.close();
    }
    @Override
    public void destroy() throws ResourceException {
        log.debug("destroy()");
        closeDatabase();
    }
    @Override
    public void cleanup() throws ResourceException {
        log.debug("cleanup()");
        this.connection.setValid(false);
        this.connection = null;
    }
    @Override
    public void associateConnection(Object connection) throws ResourceException {
        log.debug("associateConnection()");
        this.connection = (OrientDatabaseConnectionImpl) connection;
    }
    @Override
    public void addConnectionEventListener(ConnectionEventListener listener) {
        log.trace("addConnectionEventListener()");
        synchronized (listeners) {
            listeners.add(listener);
        }
    }
    @Override
    public void removeConnectionEventListener(ConnectionEventListener listener) {
        log.trace("removeConnectionEventListener()");
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }
    @Override
    public XAResource getXAResource() throws ResourceException {
        throw new ResourceException("OrientDB resource adapter does not support XA transactions");
    }
    @Override
    public LocalTransaction getLocalTransaction() throws ResourceException {
        log.debug("getLocalTransaction()");
        return new OrientLocalTransaction();
    }
    @Override
    public ManagedConnectionMetaData getMetaData() throws ResourceException {
        return new OrientManagedConnectionMetaData();
    }
    @Override
    public void setLogWriter(PrintWriter out) throws ResourceException {
        this.logWriter = out;
    }
    @Override
    public PrintWriter getLogWriter() throws ResourceException {
        return logWriter;
    }
    private void fireConnectionEvent(int event) {
        ConnectionEvent connectionEvent = new ConnectionEvent(this, event);
        connectionEvent.setConnectionHandle(connection);
        synchronized (listeners) {
            for (ConnectionEventListener listener : this.listeners) {
                switch (event) {
                    case LOCAL_TRANSACTION_STARTED:
                        log.trace("LOCAL_TRANSACTION_STARTED: {}", listener);
                        listener.localTransactionStarted(connectionEvent);
                        break;
                    case LOCAL_TRANSACTION_COMMITTED:
                        log.trace("LOCAL_TRANSACTION_COMMITTED: {}", listener);
                        listener.localTransactionCommitted(connectionEvent);
                        break;
                    case LOCAL_TRANSACTION_ROLLEDBACK:
                        log.trace("LOCAL_TRANSACTION_ROLLEDBACK: {}", listener);
                        listener.localTransactionRolledback(connectionEvent);
                        break;
                    case CONNECTION_CLOSED:
                        log.trace("CONNECTION_CLOSED: {}", listener);
                        listener.connectionClosed(connectionEvent);
                        break;
                    case CONNECTION_ERROR_OCCURRED:
                        log.trace("CONNECTION_ERROR_OCCURRED: {}", listener);
                        listener.connectionErrorOccurred(connectionEvent);
                        break;
                    default:
                        log.trace("UNKNOWN_EVENT");
                        throw new IllegalArgumentException("Unknown event: " + event);
                }
            }
        }
    }
    /**
     * Do not close the underlying connection now, as it may be used in a container-managed
     * transaction. The connection will be closed in {@link #cleanup()}.
     */
    @Override
    public void close() {
        log.trace("close()");
        fireConnectionEvent(CONNECTION_CLOSED);
    }
    public ConnectionRequestInfo getConnectionRequestInfo() {
        return cri;
    }
}
