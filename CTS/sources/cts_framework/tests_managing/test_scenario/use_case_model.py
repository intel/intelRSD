class UseCaseAction:
    def __init__(self, username, password, request, payload, response):
        self._username = username
        self._password = password
        self._request = request
        self._payload = payload
        self._response = response


class UseCase:
    def __init__(self, name, case_type, description, action):
        self._name = name
        self._case_type = case_type
        self._description = description
        self._action = action

