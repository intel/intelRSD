/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.service.detector.endpoint;

import com.intel.rsd.service.detector.data.ServiceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import static org.springframework.http.ResponseEntity.noContent;
import static org.springframework.http.ResponseEntity.notFound;
import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping("/redfish/v1/Managers")
public class ManagerController {

    private final ServiceRepository serviceRepository;

    @Autowired
    public ManagerController(ServiceRepository serviceRepository) {
        this.serviceRepository = serviceRepository;
    }

    @GetMapping("{managerId}")
    public ResponseEntity<?> getManager(@PathVariable("managerId") String managerId) {
        if (!serviceRepository.existsById(managerId)) {
            return notFound().build();
        }
        return serviceRepository.findById(managerId)
            .map(ServiceToManagerConverter::toManager)
            .map(manager -> ok().body(manager))
            .orElseGet(() -> notFound().build());
    }

    @DeleteMapping("{managerId}")
    public ResponseEntity<?> deleteManager(@PathVariable("managerId") String managerId) {
        if (!serviceRepository.existsById(managerId)) {
            return notFound().build();
        }

        //TODO: remove certificate from TrustedCertsRepository
        serviceRepository.deleteById(managerId);
        return noContent().build();
    }
}
