/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Content.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/21 18:24:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:04:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Response.hpp"

//**************************//
//		 	CONTENT			//
//**************************//

std::string	Response::ResponseContentManager() {

	if (CODE_OK(_code)) {
		switch (METHODS_INDEX(METHOD)) {
			case M_GET: {
				return (ResponseContentGet());
			}
			case M_POST: {
				return (ResponseContentPost());
			}
			case M_DELETE: {
				return (ResponseContentDelete());
			}
			case M_ERROR: {
				return ("");
			}
		}
	}
	else if (_code == CODE_REDIRECT) {
		return (_clServer->getRedirectHTML());
	}
	else
		return (generateErrorPage());

	return ("");
}

//**************************//
//		CONTENT METHODS		//
//**************************//

std::string Response::ResponseContentGet() {
	if (USE_AUTOINDEX) {
		AutoIndex index(_clServer->getIP(), getAutoIndexPath(_filePath));
		return (index.getPage());
	}
	else {
		return (_methodMap.content);
	}
}

std::string Response::ResponseContentPost() {

	return (_methodMap.content);
	return ("");
}

std::string Response::ResponseContentDelete() {
	return (_clMethods->DeleteGetCode(_filePath, _methodMap.code));
}

//**************************//
//		CONTENT ERROR		//
//**************************//

std::string Response::getHTMLErrorPage(std::string locErrorPath) {

	try {
		HtmlChecker errorPage(locErrorPath, "/");
		std::string html_error_page = errorPage.getContentFile();

		// Cas spécial si la fonction échoue
		if (html_error_page == "")
			return (generateErrorContent());

		return (html_error_page);
	}
	catch (std::exception &e) {
		std::cerr << RED << e.what() << '\n';
	}
	return ("");
}

std::string Response::generateErrorContent() {
	std::string page;

	page += "<meta charset=\"UTF-8\">";
	page += "<h1> Error " + FT::to_string(_code) + "</h1>\r\n";
	switch (_code) {

		case 101: page += "Protocol changé";
			return (page);
		case 301: page += "Ressource déplacée à l'URL contenu dans l'en tête";
			return (page);
		case 303: page += "Aucun lien avec la ressource téléversé";
			return (page);
		case 308: page += "Ressource définitivement déplacée à l'URL contenu dans l'en tête";
			return (page);
		case 400: page += "Syntaxe invalide";
			return (page);
		case 401: page += "Action non autorisé";
			return (page);
		case 403: page += "Action interdite";
			return (page);
		case 404: page += "Page non trouvée";
			return (page);
		case 405: page += "Méthode non autorisé";
			return (page);
		case 406: page += "Action pas acceptable dû à l'en tête";
			return (page);
		case 410: page += "La ressource souhaitée n'est plus disponible";
			return (page);
		case 411: page += "Taille de page requis";
			return (page);
		case 413: page += "Payload trop large, limites atteintes";
			return (page);
		case 414: page += "URI trop longue";
			return (page);
		case 415: page += "Média non supporté";
			return (page);
		case 422: page += "Le serveur n'a pas pu réaliser les instructions demandées";
			return (page);
		case 426: page += "Protocol obsolète, amélioration requis";
			return (page);
		case 429: page += "Trop de requêtes envoyées";
			return (page);
		case 500: page += "Erreur, serveur interne";
			return (page);
		case 501: page += "Méthode non implémentée";
			return (page);
		case 503: page += "Service non valable";
			return (page);
		case 505: page += "Version HTTP non valable";
			return (page);
		case 507: page += "Stockage serveur insuffisant";
			return (page);
	}
	return (page);
}

std::string	Response::generateErrorPage() {

	if (CODE_OK(_code))
		return ("");
	else {
		t_locationNode node = NEW_LOCATIONS(_clServer).getLastValidLocation(_clRequest->getRequestConfig()["Page"]);

		std::string lastErrorLocationValid = node.path;
		std::string locErrorPath = fixSlash(node.path + "/" + node.data->getErrorPages()[_code]);
		setContentType("text/html");

		std::pair<t_locationNode, std::string> errorNode = NEW_LOCATIONS(_clServer).getErrorLocation(_locationPath, _code);

		if (IS_VALID_LOCATION(errorNode.first)) {
			if (!ERROR_PAGE(errorNode.first)[_code].empty()) {
				return (getHTMLErrorPage(errorNode.second));
			}
		} else {
			return (generateErrorContent());
		}
	}
	return ("");
}
