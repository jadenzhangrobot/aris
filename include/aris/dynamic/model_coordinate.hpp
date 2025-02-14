﻿#ifndef ARIS_DYNAMIC_MODEL_COORDINATE_H_
#define ARIS_DYNAMIC_MODEL_COORDINATE_H_

#include <aris/dynamic/model_basic.hpp>

namespace aris::dynamic
{
	/// @defgroup dynamic_model_group 动力学建模模块
	/// @{
	///

	class Part;
	class Geometry;

	class ARIS_API Coordinate :public DynEle{
	public:
		auto virtual pm() const noexcept->const double4x4& = 0;
		auto virtual vs() const noexcept->const double6& = 0;
		auto virtual as() const noexcept->const double6& = 0;
		auto getPp(double *pp)const noexcept->void;
		auto getPp(const Coordinate &relative_to, double *pp)const noexcept->void;
		auto getRe(double *re, const char *type = "313")const noexcept->void;
		auto getRe(const Coordinate &relative_to, double *re, const char *type = "313")const noexcept->void;
		auto getRq(double *rq)const noexcept->void;
		auto getRq(const Coordinate &relative_to, double *rq)const noexcept->void;
		auto getRm(double *rm, Size rm_ld = 3)const noexcept->void;
		auto getRm(const Coordinate &relative_to, double *rm, Size rm_ld = 3)const noexcept->void;
		auto getPe(double *pe, const char *type = "313")const noexcept->void;
		auto getPe(const Coordinate &relative_to, double *pe, const char *type = "313")const noexcept->void;
		auto getPq(double *pq)const noexcept->void;
		auto getPq(const Coordinate &relative_to, double *pq)const noexcept->void;
		auto getPm(double *pm)const noexcept->void;
		auto getPm(const Coordinate &relative_to, double *pm)const noexcept->void;
		auto getVp(double *vp, double *pp = nullptr)const noexcept->void;
		auto getVp(const Coordinate &relative_to, double *vp, double *pp = nullptr)const noexcept->void;
		auto getWe(double *we, double *re = nullptr, const char *type = "313")const noexcept->void;
		auto getWe(const Coordinate &relative_to, double *we, double *re = nullptr, const char *type = "313")const noexcept->void;
		auto getWq(double *wq, double *rq = nullptr)const noexcept->void;
		auto getWq(const Coordinate &relative_to, double *wq, double *rq = nullptr)const noexcept->void;
		auto getWm(double *wm, double *rm = nullptr, Size wm_ld = 3, Size rm_ld = 3)const noexcept->void;
		auto getWm(const Coordinate &relative_to, double *wm, double *rm = nullptr, Size wm_ld = 3, Size rm_ld = 3)const noexcept->void;
		auto getWa(double *wa, double *rm = nullptr, Size rm_ld = 3)const noexcept->void;
		auto getWa(const Coordinate &relative_to, double *wa, double *rm = nullptr, Size rm_ld = 3)const noexcept->void;
		auto getVe(double *ve, double *pe = nullptr, const char *type = "313")const noexcept->void;
		auto getVe(const Coordinate &relative_to, double *ve, double *pe = nullptr, const char *type = "313")const noexcept->void;
		auto getVq(double *vq, double *pq = nullptr)const noexcept->void;
		auto getVq(const Coordinate &relative_to, double *vq, double *pq = nullptr)const noexcept->void;
		auto getVm(double *vm, double *pm = nullptr)const noexcept->void;
		auto getVm(const Coordinate &relative_to, double *vm, double *pm = nullptr)const noexcept->void;
		auto getVa(double *va, double *pp = nullptr)const noexcept->void;
		auto getVa(const Coordinate &relative_to, double *va, double *pp = nullptr)const noexcept->void;
		auto getVs(double *vs, double *pm = nullptr)const noexcept->void;
		auto getVs(const Coordinate &relative_to, double *vs, double *pm = nullptr)const noexcept->void;
		auto getAp(double *ap, double *vp = nullptr, double *pp = nullptr)const noexcept->void;
		auto getAp(const Coordinate &relative_to, double *ap, double *vp = nullptr, double *pp = nullptr)const noexcept->void;
		auto getXe(double *xe, double *we = nullptr, double *re = nullptr, const char *type = "313")const noexcept->void;
		auto getXe(const Coordinate &relative_to, double *xe, double *we = nullptr, double *re = nullptr, const char *type = "313")const noexcept->void;
		auto getXq(double *xq, double *wq = nullptr, double *rq = nullptr)const noexcept->void;
		auto getXq(const Coordinate &relative_to, double *xq, double *wq = nullptr, double *rq = nullptr)const noexcept->void;
		auto getXm(double *xm, double *wm = nullptr, double *rm = nullptr, Size xm_ld = 3, Size wm_ld = 3, Size rm_ld = 3)const noexcept->void;
		auto getXm(const Coordinate &relative_to, double *xm, double *wm = nullptr, double *rm = nullptr, Size xm_ld = 3, Size wm_ld = 3, Size rm_ld = 3)const noexcept->void;
		auto getXa(double *xa, double *wa = nullptr, double *rm = nullptr, Size rm_ld = 3)const noexcept->void;
		auto getXa(const Coordinate &relative_to, double *xa, double *wa = nullptr, double *rm = nullptr, Size rm_ld = 3)const noexcept->void;
		auto getAe(double *ae, double *ve = nullptr, double *pe = nullptr, const char *type = "313")const noexcept->void;
		auto getAe(const Coordinate &relative_to, double *ae, double *ve = nullptr, double *pe = nullptr, const char *type = "313")const noexcept->void;
		auto getAq(double *aq, double *vq = nullptr, double *pq = nullptr)const noexcept->void;
		auto getAq(const Coordinate &relative_to, double *aq, double *vq = nullptr, double *pq = nullptr)const noexcept->void;
		auto getAm(double *am, double *vm = nullptr, double *pm = nullptr)const noexcept->void;
		auto getAm(const Coordinate &relative_to, double *am, double *vm = nullptr, double *pm = nullptr)const noexcept->void;
		auto getAa(double *aa, double *va = nullptr, double *pp = nullptr)const noexcept->void;
		auto getAa(const Coordinate &relative_to, double *aa, double *va = nullptr, double *pp = nullptr)const noexcept->void;
		auto getAs(double *as, double *vs = nullptr, double *pm = nullptr)const noexcept->void;
		auto getAs(const Coordinate &relative_to, double *as, double *vs = nullptr, double *pm = nullptr)const noexcept->void;

		virtual ~Coordinate() = default;
		explicit Coordinate(const std::string &name = "coordinate", bool active = true);
		ARIS_DEFINE_BIG_FOUR(Coordinate);
	};
	class ARIS_API Marker final :public Coordinate{
	public:
		auto virtual pm() const noexcept->const double4x4& override;
		auto virtual vs() const noexcept->const double6& override;
		auto virtual as() const noexcept->const double6& override;
		auto setPe(const double *pe, const char *type = "313") noexcept->void;
		auto setPe(const Coordinate &relative_to, const double *pe, const char *type = "313") noexcept->void;
		auto setPq(const double *pq) noexcept->void;
		auto setPq(const Coordinate &relative_to, const double *pq) noexcept->void;
		auto setPm(const double *pm) noexcept->void;
		auto setPm(const Coordinate &relative_to, const double *pm) noexcept->void;
		auto prtPm()const noexcept->const double4x4&;
		auto setPrtPm(const double *prt_pm) noexcept->void { std::copy_n(prt_pm, 16, const_cast<double *>(*this->prtPm())); }
		auto setPrtPe(const double *prt_pe, const char *type = "313") noexcept->void { s_pe2pm(prt_pe, const_cast<double *>(*prtPm()), type); }
		auto setPrtPq(const double *prt_pq) noexcept->void { s_pq2pm(prt_pq, const_cast<double *>(*prtPm())); }
		auto fatherPart() noexcept->Part&;
		auto fatherPart() const noexcept->const Part& { return const_cast<std::decay_t<decltype(*this)>*>(this)->fatherPart(); }

		virtual ~Marker();
		explicit Marker(const std::string &name = "marker", const double *prt_pm = nullptr, bool active = true);
		ARIS_DECLARE_BIG_FOUR(Marker);

	private:
		struct Imp;
		aris::core::ImpPtr<Imp> imp_;
		friend class Model;
		friend class Part;
	};
	class ARIS_API Part final :public Coordinate{
	public:
		auto virtual pm() const noexcept->const double4x4& override;
		auto virtual vs() const noexcept->const double6& override;
		auto virtual as() const noexcept->const double6& override;
		auto prtIv() const noexcept->const double10&;
		auto setPrtIv(const double *iv, const double *pm_relative_to_part = nullptr) noexcept->void;
		auto resetMarkerPool(aris::core::PointerArray<Marker, Element> *pool)->void;
		auto markerPool()->aris::core::PointerArray<Marker, Element>&;
		auto markerPool()const->const aris::core::PointerArray<Marker, Element>&;
		auto resetGeometryPool(aris::core::PointerArray<Geometry, Element> *pool)->void;
		auto geometryPool() ->aris::core::PointerArray<Geometry, Element>&;
		auto geometryPool()const->const aris::core::PointerArray<Geometry, Element>&;
		auto addMarker(const std::string &name = "marker", const double *prt_pm = nullptr, bool active = true)->Marker&;
		auto findMarker(std::string_view name)->Marker*;
		auto setPp(const double *pp) noexcept->void;
		auto setPp(const Coordinate &relative_to, const double *pp) noexcept->void;
		auto setRe(const double *re, const char *type = "313") noexcept->void;
		auto setRe(const Coordinate &relative_to, const double *re, const char *type = "313") noexcept->void;
		auto setRq(const double *rq) noexcept->void;
		auto setRq(const Coordinate &relative_to, const double *rq) noexcept->void;
		auto setRm(const double *rm, Size rm_ld = 3) noexcept->void;
		auto setRm(const Coordinate &relative_to, const double *rm, Size rm_ld = 3) noexcept->void;
		auto setPe(const double *pe, const char *type = "313") noexcept->void;
		auto setPe(const Coordinate &relative_to, const double *pe, const char *type = "313") noexcept->void;
		auto setPq(const double *pq) noexcept->void;
		auto setPq(const Coordinate &relative_to, const double *pq) noexcept->void;
		auto setPm(const double *pm) noexcept->void;
		auto setPm(const Coordinate &relative_to, const double *pm) noexcept->void;
		auto setVp(const double *vp, const double *pp = nullptr) noexcept->void;
		auto setVp(const Coordinate &relative_to, const double *vp, const double *pp = nullptr) noexcept->void;
		auto setWe(const double *we, const double *re = nullptr, const char *type = "313") noexcept->void;
		auto setWe(const Coordinate &relative_to, const double *we, const double *re = nullptr, const char *type = "313") noexcept->void;
		auto setWq(const double *wq, const double *rq = nullptr) noexcept->void;
		auto setWq(const Coordinate &relative_to, const double *wq, const double *rq = nullptr) noexcept->void;
		auto setWm(const double *wm, const double *rm = nullptr, Size wm_ld = 3, Size rm_ld = 3) noexcept->void;
		auto setWm(const Coordinate &relative_to, const double *wm, const double *rm = nullptr, Size wm_ld = 3, Size rm_ld = 3) noexcept->void;
		auto setWa(const double *wa, const double *rm = nullptr, Size rm_ld = 3) noexcept->void;
		auto setWa(const Coordinate &relative_to, const double *wa, const double *rm = nullptr, Size rm_ld = 3) noexcept->void;
		auto setVe(const double *ve, const double *pe = nullptr, const char *type = "313") noexcept->void;
		auto setVe(const Coordinate &relative_to, const double *ve, const double *pe = nullptr, const char *type = "313") noexcept->void;
		auto setVq(const double *vq, const double *pq = nullptr) noexcept->void;
		auto setVq(const Coordinate &relative_to, const double *vq, const double *pq = nullptr) noexcept->void;
		auto setVm(const double *vm, const double *pm = nullptr) noexcept->void;
		auto setVm(const Coordinate &relative_to, const double *vm, const double *pm = nullptr) noexcept->void;
		auto setVa(const double *va, const double *pp = nullptr) noexcept->void;
		auto setVa(const Coordinate &relative_to, const double *va, const double *pp = nullptr) noexcept->void;
		auto setVs(const double *vs, const double *pm = nullptr) noexcept->void;
		auto setVs(const Coordinate &relative_to, const double *vs, const double *pm = nullptr) noexcept->void;
		auto setAp(const double *ap, const double *vp = nullptr, const double *pp = nullptr) noexcept->void;
		auto setAp(const Coordinate &relative_to, const double *ap, const double *vp = nullptr, const double *pp = nullptr) noexcept->void;
		auto setXe(const double *xe, const double *we = nullptr, const double *re = nullptr, const char *type = "313") noexcept->void;
		auto setXe(const Coordinate &relative_to, const double *xe, const double *we = nullptr, const double *re = nullptr, const char *type = "313") noexcept->void;
		auto setXq(const double *xq, const double *wq = nullptr, const double *rq = nullptr) noexcept->void;
		auto setXq(const Coordinate &relative_to, const double *xq, const double *wq = nullptr, const double *rq = nullptr) noexcept->void;
		auto setXm(const double *xm, const double *wm = nullptr, const double *rm = nullptr, Size xm_ld = 3, Size wm_ld = 3, Size rm_ld = 3) noexcept->void;
		auto setXm(const Coordinate &relative_to, const double *xm, const double *vm = nullptr, const double *rm = nullptr, Size xm_ld = 3, Size wm_ld = 3, Size rm_ld = 3) noexcept->void;
		auto setXa(const double *xa, const double *wa = nullptr, const double *rm = nullptr, Size rm_ld = 3) noexcept->void;
		auto setXa(const Coordinate &relative_to, const double *xa, const double *wa = nullptr, const double *rm = nullptr, Size rm_ld = 3) noexcept->void;
		auto setAe(const double *ae, const double *ve = nullptr, const double *pe = nullptr, const char *type = "313") noexcept->void;
		auto setAe(const Coordinate &relative_to, const double *ae, const double *ve = nullptr, const double *pe = nullptr, const char *type = "313") noexcept->void;
		auto setAq(const double *aq, const double *vq = nullptr, const double *pq = nullptr) noexcept->void;
		auto setAq(const Coordinate &relative_to, const double *aq, const double *vq = nullptr, const double *pq = nullptr) noexcept->void;
		auto setAm(const double *am, const double *vm = nullptr, const double *pm = nullptr) noexcept->void;
		auto setAm(const Coordinate &relative_to, const double *am, const double *vm = nullptr, const double *pm = nullptr) noexcept->void;
		auto setAa(const double *aa, const double *va = nullptr, const double *pp = nullptr) noexcept->void;
		auto setAa(const Coordinate &relative_to, const double *aa, const double *va = nullptr, const double *pp = nullptr) noexcept->void;
		auto setAs(const double *as, const double *vs = nullptr, const double *pm = nullptr) noexcept->void;
		auto setAs(const Coordinate &relative_to, const double *as, const double *vs = nullptr, const double *pm = nullptr) noexcept->void;

		template<typename IM_TYPE>
		auto cptIm(const Coordinate &relative_to, double *im, IM_TYPE i_t)const noexcept->void
		{
			double tem[10], pm[16], im2[36];
			getPm(relative_to, pm);
			s_iv2iv(pm, prtIv(), tem);
			s_iv2im(tem, im2);
			s_mc(6, 6, im2, 6, im, i_t);
		}
		template<typename IM_TYPE>
		auto cptGlbIm(double *im, IM_TYPE i_t)const noexcept->void
		{
			double tem[10], im2[36];
			s_iv2iv(*pm(), prtIv(), tem);
			s_iv2im(tem, im2);
			s_mc(6, 6, im2, 6, im, i_t);
		}
		template<typename IM_TYPE>
		auto cptPrtIm(double *im, IM_TYPE i_t)const noexcept->void { double tem[36]; s_iv2im(prtIv(), tem); s_mc(6, 6, tem, 6, im, i_t); }
		auto cptFg(const Coordinate &relative_to, double *fg)const noexcept->void;
		auto cptGlbFg(double *fg)const noexcept->void;
		auto cptPrtFg(double *fg)const noexcept->void;
		auto cptFv(const Coordinate &relative_to, double *fv)const noexcept->void;
		auto cptGlbFv(double *fv)const noexcept->void;
		auto cptPrtFv(double *fv)const noexcept->void;
		auto cptPf(const Coordinate &relative_to, double *pf)const noexcept->void;
		auto cptGlbPf(double *pf)const noexcept->void;
		auto cptPrtPf(double *pf)const noexcept->void;
		auto cptVs(const Coordinate &relative_to, double *vs)const noexcept->void { s_inv_tv(*relative_to.pm(), this->vs(), vs); }
		auto cptGlbVs(double *vs)const noexcept->void { getVs(vs); }
		auto cptPrtVs(double *vs)const noexcept->void { s_inv_tv(*pm(), this->vs(), vs); }
		auto cptAs(const Coordinate &relative_to, double *as)const noexcept->void { s_inv_tv(*relative_to.pm(), this->as(), as); }
		auto cptGlbAs(double *as)const noexcept->void { getAs(as); }
		auto cptPrtAs(double *as)const noexcept->void { s_inv_tv(*pm(), this->as(), as); }

		virtual ~Part();
		explicit Part(const std::string &name = "part", const double *prt_iv = nullptr, const double *pm = nullptr, const double *vs = nullptr, const double *as = nullptr, bool active = true);
		Part(Part&&);
		Part& operator=(Part&&);

	private:
		struct Imp;
		aris::core::ImpPtr<Imp> imp_;
	};
	class ARIS_API Geometry :public Element{
	public:
		virtual ~Geometry() = default;
		explicit Geometry(){}
		ARIS_DEFINE_BIG_FOUR(Geometry);
	};
	class ARIS_API ParasolidGeometry final :public Geometry{
	public:
		auto prtPm()const->const double4x4&;
		auto filePath()const->const std::string &;

		virtual ~ParasolidGeometry();
		explicit ParasolidGeometry(const std::string &graphic_file_path = "", const double* prt_pm = nullptr);
		ARIS_DECLARE_BIG_FOUR(ParasolidGeometry);

	private:
		struct Imp;
		aris::core::ImpPtr<Imp> imp_;
	};
	class ARIS_API FileGeometry final :public Geometry{
	public:
		auto prtPm()const->const double4x4&;
		auto filePath()const->const std::string &;
		auto setFilePath(std::string_view path)->void;

		virtual ~FileGeometry();
		explicit FileGeometry(const std::string &graphic_file_path = "", const double* prt_pm = nullptr);
		ARIS_DECLARE_BIG_FOUR(FileGeometry);

	private:
		struct Imp;
		aris::core::ImpPtr<Imp> imp_;
	};
	class ARIS_API ShellGeometry final :public Geometry{
	public:
		auto filePath()const->const std::string &;
		auto relativeToMarker()const->const Marker&;

		virtual ~ShellGeometry();
		explicit ShellGeometry(const std::string &graphic_file_path = "", Marker* relative_to = nullptr);
		ARIS_DECLARE_BIG_FOUR(ShellGeometry);

	private:
		struct Imp;
		aris::core::ImpPtr<Imp> imp_;
	};
	/// @}
}

#endif
