package com.intel.podm.mappers;

/**
 * Interface for mapper which maps object of one class to another.
 * @param <S> Source object class
 * @param <T> Target object Class
 */
public interface Mapper<S, T> {
    /**
     * Maps source object to target object
     * @param source source object
     * @param target target object
     */
    void map(S source, T target);

    /**
     * @return class of source objects
     */
    Class<? extends S> getSourceClass();

    /**
     * @return class of target objects
     */
    Class<? extends T> getTargetClass();
}
