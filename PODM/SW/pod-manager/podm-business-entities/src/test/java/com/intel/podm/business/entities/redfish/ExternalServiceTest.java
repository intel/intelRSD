package com.intel.podm.business.entities.redfish;

import org.testng.annotations.Test;

import java.time.Duration;

import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ExternalServiceTest {

    private static final int MIN_DELAY = 1;
    private static final int DAYS = 1000;

    @Test
    public void whenCreated_shouldBeReachable() {
        ExternalService sut = new ExternalService();

        assertTrue(sut.isReachable());
        assertFalse(sut.isUnreachableLongerThan(Duration.ZERO));
        assertFalse(sut.isUnreachableLongerThan(Duration.ofDays(DAYS)));
    }

    @Test
    public void whenMarkedAsUnreachable_shouldBeUnreachable() throws InterruptedException {
        ExternalService sut = new ExternalService();

        sut.markAsNotReachable();

        Thread.sleep(MIN_DELAY);

        assertFalse(sut.isReachable());
        assertTrue(sut.isUnreachableLongerThan(Duration.ZERO));
        assertFalse(sut.isUnreachableLongerThan(Duration.ofDays(DAYS)));
    }

    @Test(dependsOnMethods = "whenMarkedAsUnreachable_shouldBeUnreachable")
    public void whenMarkedAsUnreachableAndThenReachable_shouldBeReachable() throws InterruptedException {
        ExternalService sut = new ExternalService();

        sut.markAsNotReachable();
        sut.markAsReachable();

        Thread.sleep(MIN_DELAY);

        assertTrue(sut.isReachable());
        assertFalse(sut.isUnreachableLongerThan(Duration.ZERO));
        assertFalse(sut.isUnreachableLongerThan(Duration.ofDays(DAYS)));
    }
}
