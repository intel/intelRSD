{{ define "shared.coverage.configmap" }}
apiVersion: v1
kind: ConfigMap
metadata:
  name: {{ template "shared.fullname" . }}-coverage-javaopts-config
data:
  {{- if ( eq "enabled" ( .Values.withCoverage | default ( .Values.global.withCoverage | default "disabled" ) ) ) }}
  coverage-javaopts.conf: "-javaagent:/jacoco/jacocoagent.jar=output=tcpclient,sessionid={{ include "shared.name" .}},address=jacoco-coverage-server.default,port=9410"
  {{- end }}
{{- end }}

{{- define "shared.coverage.volume" }}
{{- if ( eq "enabled" ( .Values.withCoverage | default ( .Values.global.withCoverage | default "disabled" ) ) ) }}
- name: coverage-javaopts-volume
  configMap:
    name: {{ include "shared.fullname" .}}-coverage-javaopts-config
    items:
    - key: coverage-javaopts.conf
      path: coverage-javaopts.conf
- name: shared-coverage-agentjar
  configMap:
    name: shared-coverage-agentjar-config
    items:
    - key: jacocoagent.jar
      path: jacocoagent.jar
{{- end }}
{{- end -}}

{{- define "shared.coverage.mount" }}
{{- if ( eq "enabled" ( .Values.withCoverage | default ( .Values.global.withCoverage | default "disabled" ) ) ) }}
- name: coverage-javaopts-volume
  mountPath: /app/javaopts
  readOnly: true
- name: shared-coverage-agentjar
  mountPath: /jacoco
  readOnly: true
{{- end }}
{{- end -}}
